# Backend

This directory will contain the C++ modular monolith. The executable currently
exists only to keep the build and CI path real while the transport framework is
being selected.

Planned modules:

```text
auth
projects
assets
annotations
sessions
search
ai_jobs
storage
```

The public contract is defined in `../schemas/openapi/openapi.yaml`; realtime
messages are defined in `../schemas/websocket/message.schema.json`. Transport
handlers must translate those contracts into domain commands instead of
containing business logic.
