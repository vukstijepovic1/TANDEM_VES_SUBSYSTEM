## CANopen

CANopen is a communication protocol stack and device profile specification for
embedded systems used in automation. This library allows user to send and
receive CANopen communication objects via CAN 2.0B. User is notified of
incoming packets via callback function.

For more information on protocol itself please refer to <em>CiA 301 V4.2</em>.

## CANopen supported communication objects

### CANopen NMT

CANopen devices enter the
NMT state Pre-operational directly after finishing the CANopen devices
initialization. During this NMT state CANopen device parameterization and
CAN-ID-allocation via SDO is possible. Then the CANopen devices may be switched
directly into the NMT state Operational. The NMT state machine determines the
behavior of the communication function unit.

```
+---------+---------+---------+
| COB-ID  |  Byte0  |  Byte1  |
+---------+---------+---------+
|  0x000  |   cs    | node_id |
+---------+---------+---------+
```
* cs - command cpecifier (*refer to* ***canopen_nmt_cs_e***)
* node_id - server node id (CANopen slave)

In the NMT state ***Pre-operational***, communication via SDOs is possible. PDOs
do not exist, so PDO communication is not allowed. Configuration of PDOs,
parameters and also the allocation of application objects (PDO mapping) may be
performed by a configuration application. The CANopen device may be switched
into the NMT state Operational directly by sending the NMT service start remote
node or by means of local control.

In the NMT state ***Operational*** all communication objects are active.
Transitioning to the NMT state Operational creates all PDOs; the constructor
uses the parameters as described in the object dictionary. Object dictionary
access via SDO is possible. Implementation aspects or the application state
machine however may require to limit the access to certain objects whilst being
in the NMT state Operational, e.g. an object may contain the application program
which cannot be changed during execution.

By switching a CANopen device into the NMT state ***Stopped*** it is forced to
stop the communication altogether (except node guarding and heartbeat,
if active). Furthermore, this NMT state may be used to achieve certain
application behavior. The definition of this behavior falls into the scope of
device profiles and application profiles. If there are EMCY messages triggered
in this NMT state they are pending. The most recent active EMCY reason may be
transmitted after the CANopen device transits into another NMT state.

### CANopen Sync

CANopen Sync communication object.

```
+---------+
| COB-ID  |
+---------+
|  0x080  |
+---------+
```

### CANopen PDO

The real-time data transfer is performed by means of "Process Data Objects
(PDO)". The transfer of PDO is performed with no protocol overhead. The PDO
correspond to objects in the object dictionary and provide the interface to the
application objects. Data type and mapping of application objects into a PDO is
determined by a corresponding default PDO mapping structure within the object
dictionary. If variable PDO mapping is supported the number of PDO and the
mapping of application objects into a PDO may be transmitted to a CANopen device
during the configuration process by applying the SDO services to the
corresponding objects of the object dictionary.

Number and length of PDO of a CANopen device is application specific and may be
specified within the device profile or application profile.

There are two kinds of use for PDO. The first is data transmission and the
second data reception. It is distinguished in Transmit-PDO (TPDO) and
Receive-PDO (RPDO). CANopen devices supporting TPDO are PDO producer and CANopen
devices supporting RPDO are called PDO consumer. PDO are described by the PDO
communication parameter and the PDO mapping parameter. The structure of these
data types is explained in clause 7.4.8. The PDO communication parameter
describes the communication capabilities of the PDO. The PDO mapping parameter
contains information about the contents of the PDO.

For more information please refer to <em>CiA 301 V4.2 page 31</em>.

TPDO's are reported to the user via callback, while RPDO's are sent by user.

#### CANopen PDO1Rx

```
+----------+------------------+
|  COB-ID  |      Byte0-7     |
+----------+------------------+
|  0x200   |       data       |
|+ node_id |                  |
+----------+------------------+
```
* node_id - server node id (CANopen slave)

#### CANopen PDO2Rx

```
+----------+------------------+
|  COB-ID  |      Byte0-7     |
+----------+------------------+
|  0x300   |       data       |
|+ node_id |                  |
+----------+------------------+
```
* node_id - server node id (CANopen slave)

#### CANopen PDO3Rx

```
+----------+------------------+
|  COB-ID  |      Byte0-7     |
+----------+------------------+
|  0x400   |       data       |
|+ node_id |                  |
+----------+------------------+
```
* node_id - server node id (CANopen slave)

#### CANopen PDO4Rx

```
+----------+------------------+
|  COB-ID  |      Byte0-7     |
+----------+------------------+
|  0x500   |       data       |
|+ node_id |                  |
+----------+------------------+
```
* node_id - server node id (CANopen slave)

## CANopen supported Protocols

### CANopen Protocol SDO Download
The client is using the service SDO download for transferring data from the
client to the server (owner of the object dictionary). The data, the multiplexer
(index and sub-index) of the data set, and its size are indicated to the server.

The service is confirmed. The remote result parameter will indicate the success
or failure of the request. In case of a failure, optionally the reason is
confirmed.

Current version of the library only supports expedited download. In case of an
SDO expedited download, the data of the data set identified by the multiplexer
and size is indicated to the server.

For more information please refer to <em>CiA 301 V4.2 page 48</em>.

Protocol is comprised of *request* issued from cliend and *response* returned
from the server.

```
+---------------------------------------------------------------------+
|           Protocol SDO Download Request (sent by client)            |
+----------+---------+------------------+----------+------------------+
|  COB-ID  |  Byte0  |  Byte1    Byte2  |  Byte3   |      Byte4-7     |
+----------+---------+------------------+----------+------------------+
|  0x600   |  flags  |      index       | subindex |       data       |
|+ node_id |         |                  |          |                  |
+----------+---------+------------------+----------+------------------+
```
* node_id - server node id (CANopen slave)
* index - CANopen Object Dictionary entry index.
* subindex - CANopen Object Dictionary entry subindex.
* data - CANopen Object Dictionary entry subindex.
```
+--------------------------------------------------+
|                Flags Byte (Byte0)                |
+-----------+--------+-----------+--------+--------+
|  bit7..5  |  bit4  |  bit3..2  |  bit1  |  bit0  |
+-----------+--------+-----------+--------+--------+
|    ccs    |  res   |     n     |   e    |   s    |
+-----------+--------+-----------+--------+--------+
```
* ccs - client command specifier. (css = 1 - initiate download request)
* res - reserved.
* n - number of bytes in data field which do not contain any data (*i.e.* n = 1,
data field contains 3 bytes).
* e - transfer type. (e = 1 - transfer is expedited)
* s - size indicator. (s = 1 - data size is indicated)

```
+---------------------------------------------------------------------+
|            Protocol SDO Download Reply (sent by server)             |
+----------+---------+------------------+----------+------------------+
|  COB-ID  |  Byte0  |  Byte1    Byte2  |  Byte3   |      Byte4-7     |
+----------+---------+------------------+----------+------------------+
|  0x580   |  flags  |      index       | subindex |        res       |
|+ node_id |         |                  |          |                  |
+----------+---------+------------------+----------+------------------+
```
* node_id - server node id (CANopen slave)
* index - CANopen Object Dictionary entry index.
* subindex - CANopen Object Dictionary entry subindex.
* res - reserved.
```
+-----------------------+
|  Flags Byte (Byte0)   |
+-----------+-----------+
|  bit7..5  |  bit4..0  |
+-----------+-----------+
|    scs    |    res    |
+-----------+-----------+
```
* scs - server command specifier. (scs = 3 - initiate download response)
* res - reserved.

### CANopen Protocol SDO Upload
The client is using the service SDO upload for transferring the data from the
server (owner of the object dictionary) to the client. The multiplexer (index
and sub-index) of the data set is indicated to the server.

The service is confirmed. The remote result parameter will indicate the success
or failure of the request. In case of a failure, optionally the reason is
confirmed. In case of success, the data and its size are confirmed.

Current version of the library only supports expedited upload. In case of
successful SDO expedited upload, this service concludes the upload of the data
set identified by multiplexer and the corresponding data is confirmed

For more information please refer to <em>CiA 301 V4.2 page 50</em>.

Protocol is comprised of *request* issued from cliend and *response* returned
from the server.

```
+---------------------------------------------------------------------+
|            Protocol SDO Upload Request (sent by client)             |
+----------+---------+------------------+----------+------------------+
|  COB-ID  |  Byte0  |  Byte1    Byte2  |  Byte3   |      Byte4-7     |
+----------+---------+------------------+----------+------------------+
|  0x600   |  flags  |      index       | subindex |       res        |
|+ node_id |         |                  |          |                  |
+----------+---------+------------------+----------+------------------+
```
* node_id - server node id (CANopen slave)
* index - CANopen Object Dictionary entry index.
* subindex - CANopen Object Dictionary entry subindex.
* res - reserved.
```
+-----------------------+
|  Flags Byte (Byte0)   |
+-----------+-----------+
|  bit7..5  |  bit4..0  |
+-----------+-----------+
|    ccs    |    res    |
+-----------+-----------+
```
* ccs - client command specifier. (css = 2 - initiate upload request)
* res - reserved.

```
+---------------------------------------------------------------------+
|             Protocol SDO Upload Reply (sent by server)              |
+----------+---------+------------------+----------+------------------+
|  COB-ID  |  Byte0  |  Byte1    Byte2  |  Byte3   |      Byte4-7     |
+----------+---------+------------------+----------+------------------+
|  0x580   |  flags  |      index       | subindex |       data       |
|+ node_id |         |                  |          |                  |
+----------+---------+------------------+----------+------------------+
```
* node_id - server node id (CANopen slave)
* index - CANopen Object Dictionary entry index.
* subindex - CANopen Object Dictionary entry subindex.
* data - CANopen Object Dictionary entry subindex.
```
+--------------------------------------------------+
|                Flags Byte (Byte0)                |
+-----------+--------+-----------+--------+--------+
|  bit7..5  |  bit4  |  bit3..2  |  bit1  |  bit0  |
+-----------+--------+-----------+--------+--------+
|    scs    |  res   |     n     |   e    |   s    |
+-----------+--------+-----------+--------+--------+
```
* scs - server command specifier. (scs = 2 - initiate upload response)
* res - reserved.
* n - number of bytes in data field which do not contain any data (*i.e.* n = 1,
data field contains 3 bytes).
* e - transfer type. (e = 1 - transfer is expedited)
* s - size indicator. (s = 1 - data size is indicated)

### CANopen Protocol Heartbeat

Heartbeat producer transmits a heartbeat message cyclically. One or more
heartbeat consumer receives the indication. The relationship between producer
and consumer is configurable via the object dictionary. The heartbeat consumer
guards the reception of the heartbeat within the heartbeat consumer time. If the
heartbeat is not received within the heartbeat consumer time a heartbeat event
will be generated.

For more information please refer to <em>CiA 301 V4.2 page 76</em>.

```
+-------------------+
|     Heartbeat     |
+---------+---------+
| COB-ID  |  Byte0  |
+---------+---------+
|  0x700  |  state  |
|+ node_id|         |
+---------+---------+
```

* node_id - producer node_id
* state - state of the heartbeat producer (*refer to*
***canopen_nmt_slave_state_e***)