# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: arm_comms.proto
"""Generated protocol buffer code."""
from google.protobuf.internal import enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='arm_comms.proto',
  package='',
  syntax='proto3',
  serialized_options=None,
  create_key=_descriptor._internal_create_key,
  serialized_pb=b'\n\x0f\x61rm_comms.proto\"\x8a\x01\n\x0c\x62t_interrupt\x12\x16\n\x07\x63ommand\x18\x01 \x01(\x0e\x32\x05.Mode\x12\x19\n\x11intended_duration\x18\x02 \x01(\x05\x12\x11\n\tfrequency\x18\x03 \x01(\x01\x12\x11\n\tamplitude\x18\x04 \x01(\x01\x12\x11\n\taudiodata\x18\x05 \x03(\x0c\x12\x0e\n\x06offset\x18\x06 \x01(\x01\"b\n\x0b\x62t_response\x12\x15\n\x06status\x18\x01 \x01(\x0e\x32\x05.Mode\x12\x11\n\timpedance\x18\x02 \x01(\x01\x12\x16\n\x0etotal_duration\x18\x03 \x01(\x01\x12\x11\n\tdevice_id\x18\x04 \x01(\x05\"[\n\x0flogistical_data\x12\x15\n\x06status\x18\x01 \x01(\x0e\x32\x05.Mode\x12\x19\n\x11intended_duration\x18\x02 \x01(\x05\x12\x16\n\x0etotal_duration\x18\x03 \x01(\x05*@\n\x04Mode\x12\x08\n\x04stop\x10\x00\x12\x0e\n\nstart_sine\x10\x01\x12\x0c\n\x08start_dc\x10\x02\x12\x10\n\x0cstart_custom\x10\x03\x62\x06proto3'
)

_MODE = _descriptor.EnumDescriptor(
  name='Mode',
  full_name='Mode',
  filename=None,
  file=DESCRIPTOR,
  create_key=_descriptor._internal_create_key,
  values=[
    _descriptor.EnumValueDescriptor(
      name='stop', index=0, number=0,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='start_sine', index=1, number=1,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='start_dc', index=2, number=2,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='start_custom', index=3, number=3,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=353,
  serialized_end=417,
)
_sym_db.RegisterEnumDescriptor(_MODE)

Mode = enum_type_wrapper.EnumTypeWrapper(_MODE)
stop = 0
start_sine = 1
start_dc = 2
start_custom = 3



_BT_INTERRUPT = _descriptor.Descriptor(
  name='bt_interrupt',
  full_name='bt_interrupt',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='command', full_name='bt_interrupt.command', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='intended_duration', full_name='bt_interrupt.intended_duration', index=1,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='frequency', full_name='bt_interrupt.frequency', index=2,
      number=3, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='amplitude', full_name='bt_interrupt.amplitude', index=3,
      number=4, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='audiodata', full_name='bt_interrupt.audiodata', index=4,
      number=5, type=12, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='offset', full_name='bt_interrupt.offset', index=5,
      number=6, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=20,
  serialized_end=158,
)


_BT_RESPONSE = _descriptor.Descriptor(
  name='bt_response',
  full_name='bt_response',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='status', full_name='bt_response.status', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='impedance', full_name='bt_response.impedance', index=1,
      number=2, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='total_duration', full_name='bt_response.total_duration', index=2,
      number=3, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='device_id', full_name='bt_response.device_id', index=3,
      number=4, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=160,
  serialized_end=258,
)


_LOGISTICAL_DATA = _descriptor.Descriptor(
  name='logistical_data',
  full_name='logistical_data',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='status', full_name='logistical_data.status', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='intended_duration', full_name='logistical_data.intended_duration', index=1,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='total_duration', full_name='logistical_data.total_duration', index=2,
      number=3, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=260,
  serialized_end=351,
)

_BT_INTERRUPT.fields_by_name['command'].enum_type = _MODE
_BT_RESPONSE.fields_by_name['status'].enum_type = _MODE
_LOGISTICAL_DATA.fields_by_name['status'].enum_type = _MODE
DESCRIPTOR.message_types_by_name['bt_interrupt'] = _BT_INTERRUPT
DESCRIPTOR.message_types_by_name['bt_response'] = _BT_RESPONSE
DESCRIPTOR.message_types_by_name['logistical_data'] = _LOGISTICAL_DATA
DESCRIPTOR.enum_types_by_name['Mode'] = _MODE
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

bt_interrupt = _reflection.GeneratedProtocolMessageType('bt_interrupt', (_message.Message,), {
  'DESCRIPTOR' : _BT_INTERRUPT,
  '__module__' : 'arm_comms_pb2'
  # @@protoc_insertion_point(class_scope:bt_interrupt)
  })
_sym_db.RegisterMessage(bt_interrupt)

bt_response = _reflection.GeneratedProtocolMessageType('bt_response', (_message.Message,), {
  'DESCRIPTOR' : _BT_RESPONSE,
  '__module__' : 'arm_comms_pb2'
  # @@protoc_insertion_point(class_scope:bt_response)
  })
_sym_db.RegisterMessage(bt_response)

logistical_data = _reflection.GeneratedProtocolMessageType('logistical_data', (_message.Message,), {
  'DESCRIPTOR' : _LOGISTICAL_DATA,
  '__module__' : 'arm_comms_pb2'
  # @@protoc_insertion_point(class_scope:logistical_data)
  })
_sym_db.RegisterMessage(logistical_data)


# @@protoc_insertion_point(module_scope)