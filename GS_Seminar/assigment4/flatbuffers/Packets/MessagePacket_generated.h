// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_MESSAGEPACKET_PACKETS_H_
#define FLATBUFFERS_GENERATED_MESSAGEPACKET_PACKETS_H_

#include "../flatbuffers/flatbuffers.h"

namespace Packets {

struct MessagePacket;

struct MessagePacket FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_NAME = 4,
    VT_MESSAGE = 6
  };
  const flatbuffers::String *name() const { return GetPointer<const flatbuffers::String *>(VT_NAME); }
  const flatbuffers::String *message() const { return GetPointer<const flatbuffers::String *>(VT_MESSAGE); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_NAME) &&
           verifier.Verify(name()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_MESSAGE) &&
           verifier.Verify(message()) &&
           verifier.EndTable();
  }
};

struct MessagePacketBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_name(flatbuffers::Offset<flatbuffers::String> name) { fbb_.AddOffset(MessagePacket::VT_NAME, name); }
  void add_message(flatbuffers::Offset<flatbuffers::String> message) { fbb_.AddOffset(MessagePacket::VT_MESSAGE, message); }
  MessagePacketBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  MessagePacketBuilder &operator=(const MessagePacketBuilder &);
  flatbuffers::Offset<MessagePacket> Finish() {
    auto o = flatbuffers::Offset<MessagePacket>(fbb_.EndTable(start_, 2));
    return o;
  }
};

inline flatbuffers::Offset<MessagePacket> CreateMessagePacket(flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> name = 0,
    flatbuffers::Offset<flatbuffers::String> message = 0) {
  MessagePacketBuilder builder_(_fbb);
  builder_.add_message(message);
  builder_.add_name(name);
  return builder_.Finish();
}

inline flatbuffers::Offset<MessagePacket> CreateMessagePacketDirect(flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr,
    const char *message = nullptr) {
  return CreateMessagePacket(_fbb, name ? _fbb.CreateString(name) : 0, message ? _fbb.CreateString(message) : 0);
}

inline const Packets::MessagePacket *GetMessagePacket(const void *buf) { return flatbuffers::GetRoot<Packets::MessagePacket>(buf); }

inline bool VerifyMessagePacketBuffer(flatbuffers::Verifier &verifier) { return verifier.VerifyBuffer<Packets::MessagePacket>(nullptr); }

inline void FinishMessagePacketBuffer(flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<Packets::MessagePacket> root) { fbb.Finish(root); }

}  // namespace Packets

#endif  // FLATBUFFERS_GENERATED_MESSAGEPACKET_PACKETS_H_