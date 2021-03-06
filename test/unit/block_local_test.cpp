#include <gtest/gtest.h>
#include <dm/block_local.h>
#include <dm/block_server.h>


TEST(DM_BlockLocal, constructor) {
  DM::BlockLocal b(123, NULL);
  EXPECT_EQ( 123, b.id() );
  EXPECT_EQ( 0, b.revision() );
  EXPECT_EQ( (void*) NULL, b.data() );
}

TEST(DM_BlockLocal, copy) {
  DM::BlockLocal b1(123);
  DM::BlockLocal b2(b1);

  EXPECT_EQ( b1.id(), b2.id() );
  EXPECT_EQ( b1.revision(), b2.revision() );
  EXPECT_EQ( b1.data(), b2.data() );
}

TEST(DM_BlockLocal, setter) {
  unsigned char* memory = new unsigned char[DIMBLOCK];
  unsigned char* data = new unsigned char[DIMBLOCK];
  memset(data, 0xFF, DIMBLOCK);

  DM::BlockLocal b(10, memory);
  b.revision(20);
  b.data(data);

  EXPECT_EQ( 10, b.id() );
  EXPECT_EQ( 20, b.revision() );
  EXPECT_NE( b.data(), data );
  for (int i = 0; i < b.size(); ++i) {
    ASSERT_EQ( ((unsigned char*) b.data())[i],
               data[i] );
  }
  delete[] memory;
  delete[] data;
}

TEST(DM_BlockLocal, assignment_baseclass) {
  unsigned char* memory1 = new unsigned char[DIMBLOCK];
  unsigned char* memory2 = new unsigned char[DIMBLOCK];
  DM::BlockLocal b1(1, memory1);
  DM::BlockLocal b2(2, memory2);

  b2 = b1;
  EXPECT_EQ( b1.id(), b2.id() );
  EXPECT_EQ( b1.revision(), b2.revision() );
  EXPECT_NE( b1.data(), b2.data() );
  for (int i = 0; i < b1.size(); ++i) {
    ASSERT_EQ( ((unsigned char*) b1.data())[i],
               ((unsigned char*) b2.data())[i] );
  }
  delete[] memory1;
  delete[] memory2;
}

TEST(DM_BlockLocal, assignment_localclass) {
  unsigned char* memory = new unsigned char[DIMBLOCK];
  DM::BlockLocal b1(1, memory);
  DM::BlockServer b2(2);
  b2.revision(3);

  unsigned char* data = new unsigned char[DIMBLOCK];
  b1.data(data);

  b1 = b2;
  EXPECT_EQ( b1.id(), b2.id() );
  EXPECT_EQ( b1.revision(), b2.revision() );
  EXPECT_NE( b1.data(), b2.data() );
  for (int i = 0; i < b1.size(); ++i) {
    ASSERT_EQ( ((unsigned char*) b1.data())[i],
               ((unsigned char*) b2.data())[i] );
  }
  delete[] memory;
  delete[] data;
}

TEST(DM_BlockLocal, valid) {
  DM::BlockLocal b(123, NULL);
  EXPECT_EQ( true, b.valid() );
  b.valid(false);
  EXPECT_EQ( false, b.valid() );
}

TEST(DM_BlockLocal, revision) {
  DM::BlockLocal b;

  EXPECT_EQ( 0, b.revision() );
  b.add_revision();
  EXPECT_EQ( 1, b.revision() );
}
