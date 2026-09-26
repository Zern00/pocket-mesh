#!/usr/bin/env python3

import json
from pathlib import Path

import yaml
from jsonschema import Draft202012Validator
from openapi_spec_validator import validate_spec


ROOT = Path(__file__).resolve().parents[1]


def validate_openapi() -> None:
    path = ROOT / "schemas" / "openapi" / "openapi.yaml"
    with path.open(encoding="utf-8") as source:
        document = yaml.safe_load(source)
    validate_spec(document)
    print(f"validated {path.relative_to(ROOT)}")


def validate_websocket_schema() -> None:
    path = ROOT / "schemas" / "websocket" / "message.schema.json"
    with path.open(encoding="utf-8") as source:
        schema = json.load(source)
    Draft202012Validator.check_schema(schema)
    print(f"validated {path.relative_to(ROOT)}")


if __name__ == "__main__":
    validate_openapi()
    validate_websocket_schema()
