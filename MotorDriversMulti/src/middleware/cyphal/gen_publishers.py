#!/usr/bin/env python3
"""Generate Cyphal publish helper functions for each DSDL message/service type.

Scans provided DSDL root(s) for *.dsdl files and emits a C source file
containing raw-payload convenience wrappers for:
    - Message types (subject-based) with fixed port-ID (@fixed_port_id or numeric prefix)
    - Message types (any subject) via a parametric *_subject() variant
    - Service types (request/response) with fixed port-ID

int cyphal_publish_<namespace>_<type_lower>(struct CanardInstance* ins,
                                            struct CanardTxQueue* txq,
                                            enum CanardPriority prio,
                                            const void* payload,
                                            size_t payload_size,
                                            CanardTransferID* tid,
                                            CanardMicrosecond timeout_usec);

The wrapper just forwards into cyphal_pub_message() with the fixed subject-ID.

For services, two helpers are emitted per type:

int cyphal_request_<ns>_<type_lower>(struct CanardInstance* ins,
                                     struct CanardTxQueue* txq,
                                     enum CanardPriority prio,
                                     CanardNodeID dst_node_id,
                                     const void* payload,
                                     size_t payload_size,
                                     CanardTransferID* tid,
                                     CanardMicrosecond timeout_usec);

int cyphal_respond_<ns>_<type_lower>(struct CanardInstance* ins,
                                     struct CanardTxQueue* txq,
                                     enum CanardPriority prio,
                                     CanardNodeID dst_node_id,
                                     const void* payload,
                                     size_t payload_size,
                                     CanardTransferID request_tid,
                                     CanardMicrosecond timeout_usec);

Usage:
    python gen_publishers.py --root <dsdl_root> [--root <other>] --out <c_file>

Limitations:
- Does not attempt field-level serialization; caller must provide serialized payload.
- Assumes message (not service) types (@subject_id present).
- Ignores types without @subject_id.
"""
from __future__ import annotations
import argparse
import pathlib
import re
import sys

SUBJECT_RE = re.compile(r"@subject_id\s+(\d+)")
# Cyphal v1 regulated types use @fixed_port_id for both messages and services.
FIXED_RE = re.compile(r"@fixed_port_id\s+(\d+)", re.IGNORECASE)
FIXED_ALT = re.compile(r"Fixed[-_ ]Port[-_ ]ID\s*:\s*(\d+)", re.IGNORECASE)

def snake(s: str) -> str:
    out = []
    prev_lower = False
    for ch in s:
        if ch.isupper():
            if prev_lower:
                out.append('_')
            out.append(ch.lower())
            prev_lower = False
        elif ch.isalnum():
            out.append(ch.lower())
            prev_lower = ch.islower()
        else:
            out.append('_')
            prev_lower = False
    return re.sub(r'_+', '_', ''.join(out).strip('_'))

def derive_func_name(dsdl_path: pathlib.Path, type_name: str, root_base: pathlib.Path) -> str:
    # Include namespace components (excluding version) for uniqueness.
    parts = [p for p in dsdl_path.parent.relative_to(root_base).parts]
    base = '_'.join(snake(p) for p in parts)
    tn = snake(type_name)
    if base:
        return f"cyphal_publish_{base}_{tn}"
    return f"cyphal_publish_{tn}"

def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', action='append', required=True, help='DSDL root directory to scan')
    ap.add_argument('--out', required=True, help='Path to output C file')
    args = ap.parse_args(argv)

    roots = [pathlib.Path(r).resolve() for r in args.root]
    # Detect if generating for regulated UAVCAN namespace to add types.h
    roots_norm = [str(r).replace('\\', '/').lower() for r in roots]
    add_types_include = any(
        s.endswith('/messages/cyphal/uavcan') or s.endswith('/messages/cyphal/uavcan/') or s.endswith('/cyphal/uavcan')
        for s in roots_norm
    )
    # Prefer robust parsing via pydsdl if available.
    fixed_entries: list[tuple[str, str]] = []   # (func_name, subject_id) messages
    param_entries: list[str] = []               # message functions (parametric subject)
    service_entries: list[tuple[str, str]] = [] # (base_func_name, port_id) for services
    try:
        import pydsdl  # type: ignore
        # Compose read map for multiple roots
        lookup = pydsdl.CompositeTypeResolver.new_default()
        all_defs: list[pydsdl.SerializableType] = []
        for r in roots:
            if not r.exists():
                print(f"warning: root does not exist: {r}", file=sys.stderr)
                continue
            env = pydsdl.read_namespace(str(r), [], lookup)
            all_defs.extend(env)
        seen_fixed: set[str] = set()
        seen_param: set[str] = set()
        for t in all_defs:
            # Messages and services
            try:
                if isinstance(t, pydsdl.CompositeType):
                    if t.source_file_path:
                        fpath = pathlib.Path(t.source_file_path)
                        type_name = t.short_name
                        rb_base = roots[0]
                        for rb in roots:
                            try:
                                fpath.relative_to(rb)
                                rb_base = rb
                                break
                            except Exception:
                                continue
                        func_name = derive_func_name(fpath, type_name, rb_base)
                        if t.category == pydsdl.Category.MESSAGE:
                            # Always add a parametric subject variant (dedupe by name)
                            if func_name not in seen_param:
                                param_entries.append(func_name)
                                seen_param.add(func_name)
                            # Add fixed entry if available OR derive from MAVLink comment
                            derived_sid: str | None = None
                            if t.has_fixed_port_id:
                                derived_sid = str(t.fixed_port_id)
                            else:
                                try:
                                    txt = fpath.read_text(encoding='utf-8', errors='ignore')
                                    m = re.search(r"^#\s*Original MAVLink ID:\s*(\d+)\s*$", txt, re.MULTILINE)
                                    if m:
                                        derived_sid = m.group(1)
                                except Exception:
                                    pass
                            if derived_sid is not None and func_name not in seen_fixed:
                                fixed_entries.append((func_name, derived_sid))
                                seen_fixed.add(func_name)
                        elif t.category == pydsdl.Category.SERVICE:
                            if t.has_fixed_port_id and func_name not in seen_fixed:
                                # Reuse func_name as base for request/response wrappers
                                service_entries.append((func_name, str(t.fixed_port_id)))
                                seen_fixed.add(func_name)
            except Exception:
                continue
    except Exception:
        # Fallback: regex scan for fixed port-id/subject-id markers without pydsdl
        dsdl_files = []
        for r in roots:
            if not r.exists():
                print(f"warning: root does not exist: {r}", file=sys.stderr)
                continue
            for f in r.rglob('*.dsdl'):
                dsdl_files.append(f)
        subj_alt = re.compile(r"Subject[-_ ]ID\s*:\s*(\d+)", re.IGNORECASE)
        seen_fixed: set[str] = set()
        seen_param: set[str] = set()
        for f in sorted(dsdl_files):
            try:
                txt = f.read_text(encoding='utf-8', errors='ignore')
            except Exception as e:
                print(f"warning: cannot read {f}: {e}", file=sys.stderr)
                continue
            is_service = bool(re.search(r"^---$", txt, re.MULTILINE))
            # Try to locate a fixed port ID (preferred) or legacy subject ID marker.
            m = FIXED_RE.search(txt) or FIXED_ALT.search(txt) or SUBJECT_RE.search(txt) or subj_alt.search(txt)
            if not m:
                m = re.search(r"^#\s*Original MAVLink ID:\s*(\d+)\s*$", txt, re.MULTILINE)
            subject_id = m.group(1) if m else None
            # Also infer fixed port ID from regulated filename prefix like '7509.Name.1.0.dsdl'
            if subject_id is None:
                stem = f.name  # include dots
                # Match leading digits followed by a dot (e.g., 7509.Heartbeat.1.0.dsdl)
                mfn = re.match(r"^(\d+)\.", stem)
                if mfn:
                    subject_id = mfn.group(1)
            stem = f.stem
            # Prefer the human-readable short type name derived from filename.
            # Patterns:
            #  - "<port>.<ShortName>.<major>.<minor>.dsdl" (regulated)
            #  - "<ShortName>.<major>.<minor>.dsdl"
            #  - fallback to the whole stem
            m_reg = re.match(r"^(\d+)\.([^.]+)\.\d+\.\d+$", stem)
            m_nv = re.match(r"^([^.]+)\.\d+\.\d+$", stem)
            if m_reg:
                type_name = m_reg.group(2)
            elif m_nv:
                type_name = m_nv.group(1)
            else:
                type_name = stem.split('.')[0]
            rb_base = roots[0]
            for rb in roots:
                try:
                    f.relative_to(rb)
                    rb_base = rb
                    break
                except Exception:
                    continue
            func_name = derive_func_name(f, type_name, rb_base)
            if is_service:
                if subject_id is not None and func_name not in seen_fixed:
                    service_entries.append((func_name, subject_id))
                    seen_fixed.add(func_name)
            else:
                if func_name not in seen_param:
                    param_entries.append(func_name)
                    seen_param.add(func_name)
                if subject_id is not None and func_name not in seen_fixed:
                    fixed_entries.append((func_name, subject_id))
                    seen_fixed.add(func_name)

    out_path = pathlib.Path(args.out)
    out_path.parent.mkdir(parents=True, exist_ok=True)

    # Also generate a matching header with prototypes.
    hdr_path = out_path.with_suffix('.h')

    # Write header first so it exists for includes.
    with hdr_path.open('w', encoding='utf-8') as hp:
        hp.write('/* Auto-generated Cyphal publish helper prototypes. */\n')
        hp.write('#pragma once\n')
        hp.write('#include <stddef.h>\n')
        hp.write('#include "canard.h"\n')
        hp.write('\n')
        # Message Fixed-ID variants
        for func_name, _sid in fixed_entries:
            hp.write(f'int {func_name}(struct CanardInstance* ins, struct CanardTxQueue* txq, ')
            hp.write('enum CanardPriority prio, const void* payload, size_t payload_size, ')
            hp.write('CanardTransferID* tid, CanardMicrosecond timeout_usec);\n')
            # Directed (dst_node_id) variant for API uniformity; ignored for messages per Cyphal spec
            hp.write(f'int {func_name}_to(struct CanardInstance* ins, struct CanardTxQueue* txq, ')
            hp.write('enum CanardPriority prio, CanardNodeID dst_node_id, const void* payload, size_t payload_size, ')
            hp.write('CanardTransferID* tid, CanardMicrosecond timeout_usec);\n')
        # Parametric subject-ID variants
        for func_name in param_entries:
            hp.write(f'int {func_name}_subject(struct CanardInstance* ins, struct CanardTxQueue* txq, ')
            hp.write('enum CanardPriority prio, CanardPortID subject_id, const void* payload, size_t payload_size, ')
            hp.write('CanardTransferID* tid, CanardMicrosecond timeout_usec);\n')
            # Directed (dst_node_id) variant for API uniformity; ignored for messages per Cyphal spec
            hp.write(f'int {func_name}_subject_to(struct CanardInstance* ins, struct CanardTxQueue* txq, ')
            hp.write('enum CanardPriority prio, CanardPortID subject_id, CanardNodeID dst_node_id, const void* payload, size_t payload_size, ')
            hp.write('CanardTransferID* tid, CanardMicrosecond timeout_usec);\n')
        # Service variants (request/response)
        for base_name, _sid in service_entries:
            hp.write(f'int cyphal_request_{base_name}(struct CanardInstance* ins, struct CanardTxQueue* txq, ')
            hp.write('enum CanardPriority prio, CanardNodeID dst_node_id, const void* payload, size_t payload_size, ')
            hp.write('CanardTransferID* tid, CanardMicrosecond timeout_usec);\n')
            hp.write(f'int cyphal_respond_{base_name}(struct CanardInstance* ins, struct CanardTxQueue* txq, ')
            hp.write('enum CanardPriority prio, CanardNodeID dst_node_id, const void* payload, size_t payload_size, ')
            hp.write('CanardTransferID request_tid, CanardMicrosecond timeout_usec);\n')

    with out_path.open('w', encoding='utf-8') as fp:
        fp.write('/* Auto-generated Cyphal publish helpers. */\n')
        fp.write('#include "cyphal_utility.h"\n')
        fp.write('#include "canard.h"\n')
        fp.write('#include "util.h"\n')  # For HAL_GetTimeUS prototype
        if add_types_include:
            fp.write('#include "types.h"\n')
        fp.write('#include <string.h>\n')
        fp.write('\n')
        # Fixed-ID wrappers
        for func_name, sid in fixed_entries:
            fp.write(f'int {func_name}(struct CanardInstance* ins, struct CanardTxQueue* txq, ')
            fp.write('enum CanardPriority prio, const void* payload, size_t payload_size, ')
            fp.write('CanardTransferID* tid, CanardMicrosecond timeout_usec) {\n')
            fp.write('    return cyphal_pub_message(ins, txq, ' + sid + ', prio, payload, payload_size, tid, timeout_usec);\n')
            fp.write('}\n\n')
            # Directed fixed-ID wrapper: sets dst_node_id in metadata (non-standard for messages)
            fp.write(f'int {func_name}_to(struct CanardInstance* ins, struct CanardTxQueue* txq, ')
            fp.write('enum CanardPriority prio, CanardNodeID dst_node_id, const void* payload, size_t payload_size, ')
            fp.write('CanardTransferID* tid, CanardMicrosecond timeout_usec) {\n')
            fp.write('    return cyphal_pub_message_to(ins, txq, ' + sid + ', prio, dst_node_id, payload, payload_size, tid, timeout_usec);\n')
            fp.write('}\n\n')
        # Parametric subject-ID wrappers
        for func_name in param_entries:
            fp.write(f'int {func_name}_subject(struct CanardInstance* ins, struct CanardTxQueue* txq, ')
            fp.write('enum CanardPriority prio, CanardPortID subject_id, const void* payload, size_t payload_size, ')
            fp.write('CanardTransferID* tid, CanardMicrosecond timeout_usec) {\n')
            fp.write('    return cyphal_pub_message(ins, txq, subject_id, prio, payload, payload_size, tid, timeout_usec);\n')
            fp.write('}\n\n')
            # Directed parametric wrapper: sets dst_node_id in metadata (non-standard for messages)
            fp.write(f'int {func_name}_subject_to(struct CanardInstance* ins, struct CanardTxQueue* txq, ')
            fp.write('enum CanardPriority prio, CanardPortID subject_id, CanardNodeID dst_node_id, const void* payload, size_t payload_size, ')
            fp.write('CanardTransferID* tid, CanardMicrosecond timeout_usec) {\n')
            fp.write('    return cyphal_pub_message_to(ins, txq, subject_id, prio, dst_node_id, payload, payload_size, tid, timeout_usec);\n')
            fp.write('}\n\n')
        # Service wrappers
        for base_name, sid in service_entries:
            # Request wrapper (increments TID via cyphal_pub_service)
            fp.write(f'int cyphal_request_{base_name}(struct CanardInstance* ins, struct CanardTxQueue* txq, ')
            fp.write('enum CanardPriority prio, CanardNodeID dst_node_id, const void* payload, size_t payload_size, ')
            fp.write('CanardTransferID* tid, CanardMicrosecond timeout_usec) {\n')
            fp.write('    return cyphal_pub_service(ins, txq, CanardTransferKindRequest, ' + sid + ', dst_node_id, prio, payload, payload_size, tid, timeout_usec);\n')
            fp.write('}\n\n')
            # Response wrapper (echoes request transfer-ID; no increment)
            fp.write(f'int cyphal_respond_{base_name}(struct CanardInstance* ins, struct CanardTxQueue* txq, ')
            fp.write('enum CanardPriority prio, CanardNodeID dst_node_id, const void* payload, size_t payload_size, ')
            fp.write('CanardTransferID request_tid, CanardMicrosecond timeout_usec) {\n')
            fp.write('    const CanardMicrosecond now = (CanardMicrosecond)HAL_GetTimeUS();\n')
            fp.write('    const CanardMicrosecond deadline = now + timeout_usec;\n')
            fp.write('    const struct CanardTransferMetadata meta = {\n')
            fp.write('        .priority = prio,\n')
            fp.write('        .transfer_kind = CanardTransferKindResponse,\n')
            fp.write('        .port_id = ' + sid + ',\n')
            fp.write('        .remote_node_id = dst_node_id,\n')
            fp.write('        .transfer_id = request_tid,\n')
            fp.write('    };\n')
            fp.write('    const struct CanardPayload pl = { .size = payload_size, .data = (void*)payload };\n')
            fp.write('    return canardTxPush(txq, ins, deadline, &meta, pl, now, NULL);\n')
            fp.write('}\n\n')

    print(f"Generated {len(fixed_entries)} msg fixed-ID, {len(param_entries)} msg parametric, {len(service_entries)} services -> {out_path} / {hdr_path}")
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
