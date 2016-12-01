// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_ROOMDATA_DATA_H_
#define FLATBUFFERS_GENERATED_ROOMDATA_DATA_H_

#include "../flatbuffers/flatbuffers.h"

namespace Data {

struct RoomData;

struct RoomData FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_NUMBER = 4,
    VT_ADDRESS = 6
  };
  int32_t number() const { return GetField<int32_t>(VT_NUMBER, 0); }
  const flatbuffers::String *address() const { return GetPointer<const flatbuffers::String *>(VT_ADDRESS); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_NUMBER) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_ADDRESS) &&
           verifier.Verify(address()) &&
           verifier.EndTable();
  }
};

struct RoomDataBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_number(int32_t number) { fbb_.AddElement<int32_t>(RoomData::VT_NUMBER, number, 0); }
  void add_address(flatbuffers::Offset<flatbuffers::String> address) { fbb_.AddOffset(RoomData::VT_ADDRESS, address); }
  RoomDataBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  RoomDataBuilder &operator=(const RoomDataBuilder &);
  flatbuffers::Offset<RoomData> Finish() {
    auto o = flatbuffers::Offset<RoomData>(fbb_.EndTable(start_, 2));
    return o;
  }
};

inline flatbuffers::Offset<RoomData> CreateRoomData(flatbuffers::FlatBufferBuilder &_fbb,
    int32_t number = 0,
    flatbuffers::Offset<flatbuffers::String> address = 0) {
  RoomDataBuilder builder_(_fbb);
  builder_.add_address(address);
  builder_.add_number(number);
  return builder_.Finish();
}

inline flatbuffers::Offset<RoomData> CreateRoomDataDirect(flatbuffers::FlatBufferBuilder &_fbb,
    int32_t number = 0,
    const char *address = nullptr) {
  return CreateRoomData(_fbb, number, address ? _fbb.CreateString(address) : 0);
}

inline const Data::RoomData *GetRoomData(const void *buf) { return flatbuffers::GetRoot<Data::RoomData>(buf); }

inline bool VerifyRoomDataBuffer(flatbuffers::Verifier &verifier) { return verifier.VerifyBuffer<Data::RoomData>(nullptr); }

inline void FinishRoomDataBuffer(flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<Data::RoomData> root) { fbb.Finish(root); }

}  // namespace Data

#endif  // FLATBUFFERS_GENERATED_ROOMDATA_DATA_H_