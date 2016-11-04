// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_TEST_TEST_H_
#define FLATBUFFERS_GENERATED_TEST_TEST_H_

#include "flatbuffers/flatbuffers.h"

namespace test {

struct position;

struct position FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_X = 4,
    VT_Y = 6,
    VT_Z = 8
  };
  float x() const { return GetField<float>(VT_X, 0.0f); }
  float y() const { return GetField<float>(VT_Y, 0.0f); }
  float z() const { return GetField<float>(VT_Z, 0.0f); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_X) &&
           VerifyField<float>(verifier, VT_Y) &&
           VerifyField<float>(verifier, VT_Z) &&
           verifier.EndTable();
  }
};

struct positionBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_x(float x) { fbb_.AddElement<float>(position::VT_X, x, 0.0f); }
  void add_y(float y) { fbb_.AddElement<float>(position::VT_Y, y, 0.0f); }
  void add_z(float z) { fbb_.AddElement<float>(position::VT_Z, z, 0.0f); }
  positionBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  positionBuilder &operator=(const positionBuilder &);
  flatbuffers::Offset<position> Finish() {
    auto o = flatbuffers::Offset<position>(fbb_.EndTable(start_, 3));
    return o;
  }
};

inline flatbuffers::Offset<position> Createposition(flatbuffers::FlatBufferBuilder &_fbb,
    float x = 0.0f,
    float y = 0.0f,
    float z = 0.0f) {
  positionBuilder builder_(_fbb);
  builder_.add_z(z);
  builder_.add_y(y);
  builder_.add_x(x);
  return builder_.Finish();
}

inline const test::position *Getposition(const void *buf) { return flatbuffers::GetRoot<test::position>(buf); }

inline bool VerifypositionBuffer(flatbuffers::Verifier &verifier) { return verifier.VerifyBuffer<test::position>(nullptr); }

inline void FinishpositionBuffer(flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<test::position> root) { fbb.Finish(root); }

}  // namespace test

#endif  // FLATBUFFERS_GENERATED_TEST_TEST_H_