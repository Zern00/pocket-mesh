# API contracts

`openapi/openapi.yaml` is the provisional public HTTP contract. All Android
traffic goes through this API; database, AI, and object storage interfaces are
internal implementation details.

`websocket/message.schema.json` describes messages sent after a client joins a
review session. Every message carries a protocol version, type, session ID,
message ID, and timestamp.

## Realtime semantics

Messages are not all equally durable:

| Message | Direction | Delivery semantics |
| --- | --- | --- |
| `join_session` | client -> server | acknowledged by `session_snapshot` |
| `pointer_move` | both | ephemeral; latest sequence wins |
| `selection_changed` | both | ephemeral; latest sequence wins |
| `annotation_created` | server -> client | persistent; REST write is authoritative |
| `session_snapshot` | server -> client | replaces local presence state |
| `error` | server -> client | correlated through `messageId` when possible |

Annotations are created through REST first. WebSocket only broadcasts the
committed annotation, avoiding two competing persistence paths in the MVP.

## Compatibility

The initial protocol version is `1`. Receivers must ignore unknown JSON fields.
Adding optional fields is compatible. Renaming/removing fields or changing
their meaning requires a protocol version change after the first client ships.
