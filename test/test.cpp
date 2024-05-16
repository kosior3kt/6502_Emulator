#include "test.hpp"

TEST_F(TEST_6502, MemoryDumpTest)
{
   mem_.debug_dumpMemory("dupa.txt");
}

TEST_F(TEST_6502, LDA_IM)
{
   constexpr uint8_t ASSIGNED_CYCLES = 2;
   mem_.debug_set(0xFFFC, CPU::INS_LDA_IM);
   mem_.debug_set(0xFFFD, 0x69);
   auto cyclesLeft = cpu_.execute(ASSIGNED_CYCLES, mem_);

   // Byte targetCycles =
   // cpu_.numberOfCyclesByInstruction_.find(CPU::INS_LDA_IM);
   // EXPECT_EQ(ASSIGNED_CYCLES, targetCycles); ///make everything into that
   // later. Maybe calculate number of cycles based on some functino(?)
   EXPECT_EQ(cyclesLeft, 0);
   EXPECT_EQ((int)cpu_.A, 0x69);
   EXPECT_FALSE((int)cpu_.Z);
   EXPECT_FALSE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

TEST_F(TEST_6502, LDA_IM_NEGATIVE)
{
   mem_.debug_set(0xFFFC, CPU::INS_LDA_IM);
   mem_.debug_set(
       0xFFFD, 0b10000000
   ); /// the 1 on the first bit of 8bit number is consider to make it negative
      /// - 2 complements or sth
   auto cyclesLeft = cpu_.execute(2, mem_);

   EXPECT_EQ(cyclesLeft, 0);
   EXPECT_EQ((int)cpu_.A, 0b10000000);
   EXPECT_FALSE((int)cpu_.Z);
   EXPECT_TRUE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

TEST_F(TEST_6502, LDA_IM_NEGATIVE_AND_OVERRIDEN)
{
   cpu_.A = 0x69;
   mem_.debug_set(0xFFFC, CPU::INS_LDA_IM);
   mem_.debug_set(
       0xFFFD, 0b10000000
   ); /// the 1 on the first bit of 8bit number is consider to make it negative
      /// - 2 complements or sth
   auto cyclesLeft = cpu_.execute(2, mem_);

   EXPECT_EQ(cyclesLeft, 0);
   EXPECT_EQ((int)cpu_.A, 0b10000000);
   EXPECT_FALSE((int)cpu_.Z);
   EXPECT_TRUE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

TEST_F(TEST_6502, LDA_IM_ZERO_AND_OVERRIDEN)
{
   cpu_.A = 0x69;
   mem_.debug_set(0xFFFC, CPU::INS_LDA_IM);
   mem_.debug_set(
       0xFFFD, 0
   ); /// the 1 on the first bit of 8bit number is consider to make it negative
      /// - 2 complements or sth
   auto cyclesLeft = cpu_.execute(2, mem_);

   EXPECT_EQ(cyclesLeft, 0);
   EXPECT_EQ((int)cpu_.A, 0);
   EXPECT_TRUE((int)cpu_.Z);
   EXPECT_FALSE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

TEST_F(TEST_6502, LDA_ZP)
{
   mem_.debug_set(0xFFFC, CPU::INS_LDA_ZP);
   mem_.debug_set(0xFFFD, 0x42);
   mem_.debug_set(0x0042, 0x69);
   auto cyclesLeft = cpu_.execute(3, mem_);

   EXPECT_EQ(cyclesLeft, 0);
   EXPECT_EQ((int)cpu_.A, 0x69);
   EXPECT_FALSE((int)cpu_.Z);
   EXPECT_FALSE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

TEST_F(TEST_6502, LDA_ZPX)
{
   cpu_.X = 0x5;
   mem_.debug_set(0xFFFC, CPU::INS_LDA_ZPX);
   mem_.debug_set(0xFFFD, 0x42);
   mem_.debug_set(0x0047, 0x69);
   auto cyclesLeft = cpu_.execute(3, mem_);

   EXPECT_EQ(cyclesLeft, 0);
   EXPECT_EQ((int)cpu_.A, 0x69);
   EXPECT_FALSE((int)cpu_.Z);
   EXPECT_FALSE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

TEST_F(TEST_6502, LDA_ZPX_withWrapping)
{
   cpu_.X = 0xFF;
   mem_.debug_set(0xFFFC, CPU::INS_LDA_ZPX);
   mem_.debug_set(0xFFFD, 0x80);
   mem_.debug_set(0x007F, 0x69);
   auto cyclesLeft = cpu_.execute(3, mem_);

   EXPECT_EQ(cyclesLeft, 0);
   EXPECT_EQ((int)cpu_.A, 0x69);
   EXPECT_FALSE((int)cpu_.Z);
   EXPECT_FALSE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

TEST_F(TEST_6502, slightlyMoreComplicatedProgram)
{
   mem_.debug_set(0xFFFC, CPU::INS_JSR);
   mem_.debug_set(0xFFFD, 0x42);
   mem_.debug_set(0xFFFE, 0x42);
   mem_.debug_set(0x4242, CPU::INS_LDA_IM);
   mem_.debug_set(0x4243, 0x69);
   mem_.debug_set(0x4244, CPU::INS_JSR);
   mem_.debug_set(0x4245, 0x69);
   mem_.debug_set(0x4246, 0x69);
   mem_.debug_set(0x6969, CPU::INS_LDA_IM);
   mem_.debug_set(0x696A, 0x6B);
   auto cyclesLeft = cpu_.execute(16, mem_);

   EXPECT_EQ(cyclesLeft, 0);
   EXPECT_EQ((int)cpu_.A, 0x6B);
   EXPECT_FALSE((int)cpu_.Z);
   EXPECT_FALSE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

TEST_F(TEST_6502, idleCPU)
{
   auto cyclesAtTheBeggining = 69;
   auto cyclesLeft           = cpu_.execute(cyclesAtTheBeggining, mem_);

   EXPECT_EQ(
       cyclesLeft, cyclesAtTheBeggining - 1
   ); /// one cycle for fetchig instruction
   EXPECT_FALSE((int)cpu_.Z);
   EXPECT_FALSE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

TEST_F(TEST_6502, LDA_AbsoluteAddressing)
{
   mem_.debug_set(0xFFFC, CPU::INS_LDA_ABS);
   mem_.debug_set(0xFFFD, 0x80);
   mem_.debug_set(0xFFFE, 0x69); /// 0x6980
   mem_.debug_set(0x6980, 0x69);
   auto cyclesLeft = cpu_.execute(4, mem_);

   EXPECT_EQ(cyclesLeft, 0);
   EXPECT_EQ((int)cpu_.A, 0x69);
   EXPECT_FALSE((int)cpu_.Z);
   EXPECT_FALSE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

TEST_F(TEST_6502, LDAX_AbsoluteAddressing_noPageCrossing)
{
   cpu_.X = 0x0001;
   mem_.debug_set(0xFFFC, CPU::INS_LDA_ABSX);
   mem_.debug_set(0xFFFD, 0x80);
   mem_.debug_set(0xFFFE, 0x69); /// 0x6980 + 0x0001 = 0x6981
   mem_.debug_set(0x6981, 0x69);
   auto cyclesLeft = cpu_.execute(4, mem_);

   EXPECT_EQ(cyclesLeft, 0);
   EXPECT_EQ((int)cpu_.A, 0x69);
   EXPECT_FALSE((int)cpu_.Z);
   EXPECT_FALSE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

TEST_F(TEST_6502, LDAY_AbsoluteAddressing_noPageCrossing)
{
   cpu_.Y = 0x0001;
   mem_.debug_set(0xFFFC, CPU::INS_LDA_ABSY);
   mem_.debug_set(0xFFFD, 0x80);
   mem_.debug_set(0xFFFE, 0x69); /// 0x6980 + 0x0001 = 0x6981
   mem_.debug_set(0x6981, 0x69);
   auto cyclesLeft = cpu_.execute(4, mem_);

   EXPECT_EQ(cyclesLeft, 0);
   EXPECT_EQ((int)cpu_.A, 0x69);
   EXPECT_FALSE((int)cpu_.Z);
   EXPECT_FALSE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

TEST_F(TEST_6502, LDAX_AbsoluteAddressing_withPageCrossing)
{
   cpu_.X = 0x27;
   mem_.debug_set(0xFFFC, CPU::INS_LDA_ABSX);
   mem_.debug_set(0xFFFD, 0xFF);
   mem_.debug_set(0xFFFE, 0xFF); /// 0xFFFF + 0x0027 = (in this case we take into considerationg page crossing) = 0x0027 - 0x0001 (for page crossing compensation)
   mem_.debug_set(0x0026, 0x69);
   auto cyclesLeft = cpu_.execute(5, mem_);

   EXPECT_EQ(cyclesLeft, 0);
   EXPECT_EQ((int)cpu_.A, 0x69);
   EXPECT_FALSE((int)cpu_.Z);
   EXPECT_FALSE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

TEST_F(TEST_6502, LDAY_AbsoluteAddressing_withPageCrossing)
{
   cpu_.Y = 0x27;
   mem_.debug_set(0xFFFC, CPU::INS_LDA_ABSY);
   mem_.debug_set(0xFFFD, 0xFF);
   mem_.debug_set(0xFFFE, 0xFF); /// 0xFFFF + 0x0027 = (in this case we take into considerationg page crossing) = 0x0027 - 0x0001 (for page crossing compensation)
   mem_.debug_set(0x0026, 0x69);
   auto cyclesLeft = cpu_.execute(5, mem_);

   EXPECT_EQ(cyclesLeft, 0);
   EXPECT_EQ((int)cpu_.A, 0x69);
   EXPECT_FALSE((int)cpu_.Z);
   EXPECT_FALSE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

TEST_F(TEST_6502, LDA_IndirectAdressingY)
{
   cpu_.Y = 0x02;
   mem_.debug_set(0xFFFC, CPU::INS_LDA_INDY);
   mem_.debug_set(0xFFFD, 0x004);
   mem_.debug_set(0x0006, 0x10); /// 0x02 + 0x04 = 0x06
   mem_.debug_set(0x0007, 0x05); ///we take 2 bits from there and sum up the result
   mem_.debug_set(0x0510, 0x69); 
   mem_.debug_set(0x1005, 0x69); 

   auto cyclesLeft = cpu_.execute(5, mem_);

   EXPECT_EQ(cyclesLeft, 0);
   EXPECT_EQ((int)cpu_.A, 0x69);
   EXPECT_FALSE((int)cpu_.Z);
   EXPECT_FALSE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

TEST_F(TEST_6502, LDA_IndirectAdressingX)
{
   cpu_.X = 0x02;
   mem_.debug_set(0xFFFC, CPU::INS_LDA_INDX);
   mem_.debug_set(0xFFFD, 0x004);
   mem_.debug_set(0x0006, 0x10); /// 0x02 + 0x04 = 0x06
   mem_.debug_set(0x0007, 0x05); ///we take 2 bits from there and sum up the result
   mem_.debug_set(0x0510, 0x69); 

   auto cyclesLeft = cpu_.execute(5, mem_);

   EXPECT_EQ(cyclesLeft, 0);
   EXPECT_EQ((int)cpu_.A, 0x69);
   EXPECT_FALSE((int)cpu_.Z);
   EXPECT_FALSE((int)cpu_.N);
   EXPECT_TRUE(testHelper::basicFlagsUnused(cpu_, copyCPU_));
}

