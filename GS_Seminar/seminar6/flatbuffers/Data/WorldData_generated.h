// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_WORLDDATA_DATA_H_
#define FLATBUFFERS_GENERATED_WORLDDATA_DATA_H_

#include "flatbuffers/flatbuffers.h"

#include "InGameUserData_generated.h"
#include "UserData_generated.h"

namespace Data {

struct WorldData;

struct WorldData FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_USERS = 4,
    VT_MAP = 6
  };
  const flatbuffers::Vector<flatbuffers::Offset<Data::InGameUserData>> *users() const { return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<Data::InGameUserData>> *>(VT_USERS); }
  int32_t map() const { return GetField<int32_t>(VT_MAP, 0); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_USERS) &&
           verifier.Verify(users()) &&
           verifier.VerifyVectorOfTables(users()) &&
           VerifyField<int32_t>(verifier, VT_MAP) &&
           verifier.EndTable();
  }
};

struct WorldDataBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_users(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Data::InGameUserData>>> users) { fbb_.AddOffset(WorldData::VT_USERS, users); }
  void add_map(int32_t map) { fbb_.AddElement<int32_t>(WorldData::VT_MAP, map, 0); }
  WorldDataBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  WorldDataBuilder &operator=(const WorldDataBuilder &);
  flatbuffers::Offset<WorldData> Finish() {
    auto o = flatbuffers::Offset<WorldData>(fbb_.EndTable(start_, 2));
    return o;
  }
};

inline flatbuffers::Offset<WorldData> CreateWorldData(flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Data::InGameUserData>>> users = 0,
    int32_t map = 0) {
  WorldDataBuilder builder_(_fbb);
  builder_.add_map(map);
  builder_.add_users(users);
  return builder_.Finish();
}

inline flatbuffers::Offset<WorldData> CreateWorldDataDirect(flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<flatbuffers::Offset<Data::InGameUserData>> *users = nullptr,
    int32_t map = 0) {
  return CreateWorldData(_fbb, users ? _fbb.CreateVector<flatbuffers::Offset<Data::InGameUserData>>(*users) : 0, map);
}

inline const Data::WorldData *GetWorldData(const void *buf) { return flatbuffers::GetRoot<Data::WorldData>(buf); }

inline bool VerifyWorldDataBuffer(flatbuffers::Verifier &verifier) { return verifier.VerifyBuffer<Data::WorldData>(nullptr); }

inline void FinishWorldDataBuffer(flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<Data::WorldData> root) { fbb.Finish(root); }

}  // namespace Data

#endif  // FLATBUFFERS_GENERATED_WORLDDATA_DATA_H_
