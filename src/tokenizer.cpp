#include "yacas/tokenizer.h"

#include "yacas/lisperror.h"

#include "yacas/utf8.h"

#include <cstdint>
#include <set>

namespace {
    static const char symbolics[] = "~`!@#$^&*-=+:<>?/\\|";

    // Ll and Lu categories combined
    // see http://www.fileformat.info/info/unicode/category/index.htm
    static const std::set<std::uint32_t> letters = {
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a,
        0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54,
        0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x61, 0x62, 0x63, 0x64,
        0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e,
        0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
        0x79, 0x7a, 0xb5, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6,
        0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0,
        0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd8, 0xd9, 0xda, 0xdb,
        0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5,
        0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf8, 0xf9, 0xfa,
        0xfb, 0xfc, 0xfd, 0xfe, 0xff, 0x100, 0x101, 0x102, 0x103,
        0x104, 0x105, 0x106, 0x107, 0x108, 0x109, 0x10a, 0x10b, 0x10c,
        0x10d, 0x10e, 0x10f, 0x110, 0x111, 0x112, 0x113, 0x114, 0x115,
        0x116, 0x117, 0x118, 0x119, 0x11a, 0x11b, 0x11c, 0x11d, 0x11e,
        0x11f, 0x120, 0x121, 0x122, 0x123, 0x124, 0x125, 0x126, 0x127,
        0x128, 0x129, 0x12a, 0x12b, 0x12c, 0x12d, 0x12e, 0x12f, 0x130,
        0x131, 0x132, 0x133, 0x134, 0x135, 0x136, 0x137, 0x138, 0x139,
        0x13a, 0x13b, 0x13c, 0x13d, 0x13e, 0x13f, 0x140, 0x141, 0x142,
        0x143, 0x144, 0x145, 0x146, 0x147, 0x148, 0x149, 0x14a, 0x14b,
        0x14c, 0x14d, 0x14e, 0x14f, 0x150, 0x151, 0x152, 0x153, 0x154,
        0x155, 0x156, 0x157, 0x158, 0x159, 0x15a, 0x15b, 0x15c, 0x15d,
        0x15e, 0x15f, 0x160, 0x161, 0x162, 0x163, 0x164, 0x165, 0x166,
        0x167, 0x168, 0x169, 0x16a, 0x16b, 0x16c, 0x16d, 0x16e, 0x16f,
        0x170, 0x171, 0x172, 0x173, 0x174, 0x175, 0x176, 0x177, 0x178,
        0x179, 0x17a, 0x17b, 0x17c, 0x17d, 0x17e, 0x17f, 0x180, 0x181,
        0x182, 0x183, 0x184, 0x185, 0x186, 0x187, 0x188, 0x189, 0x18a,
        0x18b, 0x18c, 0x18d, 0x18e, 0x18f, 0x190, 0x191, 0x192, 0x193,
        0x194, 0x195, 0x196, 0x197, 0x198, 0x199, 0x19a, 0x19b, 0x19c,
        0x19d, 0x19e, 0x19f, 0x1a0, 0x1a1, 0x1a2, 0x1a3, 0x1a4, 0x1a5,
        0x1a6, 0x1a7, 0x1a8, 0x1a9, 0x1aa, 0x1ab, 0x1ac, 0x1ad, 0x1ae,
        0x1af, 0x1b0, 0x1b1, 0x1b2, 0x1b3, 0x1b4, 0x1b5, 0x1b6, 0x1b7,
        0x1b8, 0x1b9, 0x1ba, 0x1bc, 0x1bd, 0x1be, 0x1bf, 0x1c4, 0x1c6,
        0x1c7, 0x1c9, 0x1ca, 0x1cc, 0x1cd, 0x1ce, 0x1cf, 0x1d0, 0x1d1,
        0x1d2, 0x1d3, 0x1d4, 0x1d5, 0x1d6, 0x1d7, 0x1d8, 0x1d9, 0x1da,
        0x1db, 0x1dc, 0x1dd, 0x1de, 0x1df, 0x1e0, 0x1e1, 0x1e2, 0x1e3,
        0x1e4, 0x1e5, 0x1e6, 0x1e7, 0x1e8, 0x1e9, 0x1ea, 0x1eb, 0x1ec,
        0x1ed, 0x1ee, 0x1ef, 0x1f0, 0x1f1, 0x1f3, 0x1f4, 0x1f5, 0x1f6,
        0x1f7, 0x1f8, 0x1f9, 0x1fa, 0x1fb, 0x1fc, 0x1fd, 0x1fe, 0x1ff,
        0x200, 0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207, 0x208,
        0x209, 0x20a, 0x20b, 0x20c, 0x20d, 0x20e, 0x20f, 0x210, 0x211,
        0x212, 0x213, 0x214, 0x215, 0x216, 0x217, 0x218, 0x219, 0x21a,
        0x21b, 0x21c, 0x21d, 0x21e, 0x21f, 0x220, 0x221, 0x222, 0x223,
        0x224, 0x225, 0x226, 0x227, 0x228, 0x229, 0x22a, 0x22b, 0x22c,
        0x22d, 0x22e, 0x22f, 0x230, 0x231, 0x232, 0x233, 0x234, 0x235,
        0x236, 0x237, 0x238, 0x239, 0x23a, 0x23b, 0x23c, 0x23d, 0x23e,
        0x23f, 0x240, 0x241, 0x242, 0x243, 0x244, 0x245, 0x246, 0x247,
        0x248, 0x249, 0x24a, 0x24b, 0x24c, 0x24d, 0x24e, 0x24f, 0x250,
        0x251, 0x252, 0x253, 0x254, 0x255, 0x256, 0x257, 0x258, 0x259,
        0x25a, 0x25b, 0x25c, 0x25d, 0x25e, 0x25f, 0x260, 0x261, 0x262,
        0x263, 0x264, 0x265, 0x266, 0x267, 0x268, 0x269, 0x26a, 0x26b,
        0x26c, 0x26d, 0x26e, 0x26f, 0x270, 0x271, 0x272, 0x273, 0x274,
        0x275, 0x276, 0x277, 0x278, 0x279, 0x27a, 0x27b, 0x27c, 0x27d,
        0x27e, 0x27f, 0x280, 0x281, 0x282, 0x283, 0x284, 0x285, 0x286,
        0x287, 0x288, 0x289, 0x28a, 0x28b, 0x28c, 0x28d, 0x28e, 0x28f,
        0x290, 0x291, 0x292, 0x293, 0x295, 0x296, 0x297, 0x298, 0x299,
        0x29a, 0x29b, 0x29c, 0x29d, 0x29e, 0x29f, 0x2a0, 0x2a1, 0x2a2,
        0x2a3, 0x2a4, 0x2a5, 0x2a6, 0x2a7, 0x2a8, 0x2a9, 0x2aa, 0x2ab,
        0x2ac, 0x2ad, 0x2ae, 0x2af, 0x370, 0x371, 0x372, 0x373, 0x376,
        0x377, 0x37b, 0x37c, 0x37d, 0x37f, 0x386, 0x388, 0x389, 0x38a,
        0x38c, 0x38e, 0x38f, 0x390, 0x391, 0x392, 0x393, 0x394, 0x395,
        0x396, 0x397, 0x398, 0x399, 0x39a, 0x39b, 0x39c, 0x39d, 0x39e,
        0x39f, 0x3a0, 0x3a1, 0x3a3, 0x3a4, 0x3a5, 0x3a6, 0x3a7, 0x3a8,
        0x3a9, 0x3aa, 0x3ab, 0x3ac, 0x3ad, 0x3ae, 0x3af, 0x3b0, 0x3b1,
        0x3b2, 0x3b3, 0x3b4, 0x3b5, 0x3b6, 0x3b7, 0x3b8, 0x3b9, 0x3ba,
        0x3bb, 0x3bc, 0x3bd, 0x3be, 0x3bf, 0x3c0, 0x3c1, 0x3c2, 0x3c3,
        0x3c4, 0x3c5, 0x3c6, 0x3c7, 0x3c8, 0x3c9, 0x3ca, 0x3cb, 0x3cc,
        0x3cd, 0x3ce, 0x3cf, 0x3d0, 0x3d1, 0x3d2, 0x3d3, 0x3d4, 0x3d5,
        0x3d6, 0x3d7, 0x3d8, 0x3d9, 0x3da, 0x3db, 0x3dc, 0x3dd, 0x3de,
        0x3df, 0x3e0, 0x3e1, 0x3e2, 0x3e3, 0x3e4, 0x3e5, 0x3e6, 0x3e7,
        0x3e8, 0x3e9, 0x3ea, 0x3eb, 0x3ec, 0x3ed, 0x3ee, 0x3ef, 0x3f0,
        0x3f1, 0x3f2, 0x3f3, 0x3f4, 0x3f5, 0x3f7, 0x3f8, 0x3f9, 0x3fa,
        0x3fb, 0x3fc, 0x3fd, 0x3fe, 0x3ff, 0x400, 0x401, 0x402, 0x403,
        0x404, 0x405, 0x406, 0x407, 0x408, 0x409, 0x40a, 0x40b, 0x40c,
        0x40d, 0x40e, 0x40f, 0x410, 0x411, 0x412, 0x413, 0x414, 0x415,
        0x416, 0x417, 0x418, 0x419, 0x41a, 0x41b, 0x41c, 0x41d, 0x41e,
        0x41f, 0x420, 0x421, 0x422, 0x423, 0x424, 0x425, 0x426, 0x427,
        0x428, 0x429, 0x42a, 0x42b, 0x42c, 0x42d, 0x42e, 0x42f, 0x430,
        0x431, 0x432, 0x433, 0x434, 0x435, 0x436, 0x437, 0x438, 0x439,
        0x43a, 0x43b, 0x43c, 0x43d, 0x43e, 0x43f, 0x440, 0x441, 0x442,
        0x443, 0x444, 0x445, 0x446, 0x447, 0x448, 0x449, 0x44a, 0x44b,
        0x44c, 0x44d, 0x44e, 0x44f, 0x450, 0x451, 0x452, 0x453, 0x454,
        0x455, 0x456, 0x457, 0x458, 0x459, 0x45a, 0x45b, 0x45c, 0x45d,
        0x45e, 0x45f, 0x460, 0x461, 0x462, 0x463, 0x464, 0x465, 0x466,
        0x467, 0x468, 0x469, 0x46a, 0x46b, 0x46c, 0x46d, 0x46e, 0x46f,
        0x470, 0x471, 0x472, 0x473, 0x474, 0x475, 0x476, 0x477, 0x478,
        0x479, 0x47a, 0x47b, 0x47c, 0x47d, 0x47e, 0x47f, 0x480, 0x481,
        0x48a, 0x48b, 0x48c, 0x48d, 0x48e, 0x48f, 0x490, 0x491, 0x492,
        0x493, 0x494, 0x495, 0x496, 0x497, 0x498, 0x499, 0x49a, 0x49b,
        0x49c, 0x49d, 0x49e, 0x49f, 0x4a0, 0x4a1, 0x4a2, 0x4a3, 0x4a4,
        0x4a5, 0x4a6, 0x4a7, 0x4a8, 0x4a9, 0x4aa, 0x4ab, 0x4ac, 0x4ad,
        0x4ae, 0x4af, 0x4b0, 0x4b1, 0x4b2, 0x4b3, 0x4b4, 0x4b5, 0x4b6,
        0x4b7, 0x4b8, 0x4b9, 0x4ba, 0x4bb, 0x4bc, 0x4bd, 0x4be, 0x4bf,
        0x4c0, 0x4c1, 0x4c2, 0x4c3, 0x4c4, 0x4c5, 0x4c6, 0x4c7, 0x4c8,
        0x4c9, 0x4ca, 0x4cb, 0x4cc, 0x4cd, 0x4ce, 0x4cf, 0x4d0, 0x4d1,
        0x4d2, 0x4d3, 0x4d4, 0x4d5, 0x4d6, 0x4d7, 0x4d8, 0x4d9, 0x4da,
        0x4db, 0x4dc, 0x4dd, 0x4de, 0x4df, 0x4e0, 0x4e1, 0x4e2, 0x4e3,
        0x4e4, 0x4e5, 0x4e6, 0x4e7, 0x4e8, 0x4e9, 0x4ea, 0x4eb, 0x4ec,
        0x4ed, 0x4ee, 0x4ef, 0x4f0, 0x4f1, 0x4f2, 0x4f3, 0x4f4, 0x4f5,
        0x4f6, 0x4f7, 0x4f8, 0x4f9, 0x4fa, 0x4fb, 0x4fc, 0x4fd, 0x4fe,
        0x4ff, 0x500, 0x501, 0x502, 0x503, 0x504, 0x505, 0x506, 0x507,
        0x508, 0x509, 0x50a, 0x50b, 0x50c, 0x50d, 0x50e, 0x50f, 0x510,
        0x511, 0x512, 0x513, 0x514, 0x515, 0x516, 0x517, 0x518, 0x519,
        0x51a, 0x51b, 0x51c, 0x51d, 0x51e, 0x51f, 0x520, 0x521, 0x522,
        0x523, 0x524, 0x525, 0x526, 0x527, 0x528, 0x529, 0x52a, 0x52b,
        0x52c, 0x52d, 0x52e, 0x52f, 0x531, 0x532, 0x533, 0x534, 0x535,
        0x536, 0x537, 0x538, 0x539, 0x53a, 0x53b, 0x53c, 0x53d, 0x53e,
        0x53f, 0x540, 0x541, 0x542, 0x543, 0x544, 0x545, 0x546, 0x547,
        0x548, 0x549, 0x54a, 0x54b, 0x54c, 0x54d, 0x54e, 0x54f, 0x550,
        0x551, 0x552, 0x553, 0x554, 0x555, 0x556, 0x561, 0x562, 0x563,
        0x564, 0x565, 0x566, 0x567, 0x568, 0x569, 0x56a, 0x56b, 0x56c,
        0x56d, 0x56e, 0x56f, 0x570, 0x571, 0x572, 0x573, 0x574, 0x575,
        0x576, 0x577, 0x578, 0x579, 0x57a, 0x57b, 0x57c, 0x57d, 0x57e,
        0x57f, 0x580, 0x581, 0x582, 0x583, 0x584, 0x585, 0x586, 0x587,
        0x10a0, 0x10a1, 0x10a2, 0x10a3, 0x10a4, 0x10a5, 0x10a6,
        0x10a7, 0x10a8, 0x10a9, 0x10aa, 0x10ab, 0x10ac, 0x10ad,
        0x10ae, 0x10af, 0x10b0, 0x10b1, 0x10b2, 0x10b3, 0x10b4,
        0x10b5, 0x10b6, 0x10b7, 0x10b8, 0x10b9, 0x10ba, 0x10bb,
        0x10bc, 0x10bd, 0x10be, 0x10bf, 0x10c0, 0x10c1, 0x10c2,
        0x10c3, 0x10c4, 0x10c5, 0x10c7, 0x10cd, 0x13a0, 0x13a1,
        0x13a2, 0x13a3, 0x13a4, 0x13a5, 0x13a6, 0x13a7, 0x13a8,
        0x13a9, 0x13aa, 0x13ab, 0x13ac, 0x13ad, 0x13ae, 0x13af,
        0x13b0, 0x13b1, 0x13b2, 0x13b3, 0x13b4, 0x13b5, 0x13b6,
        0x13b7, 0x13b8, 0x13b9, 0x13ba, 0x13bb, 0x13bc, 0x13bd,
        0x13be, 0x13bf, 0x13c0, 0x13c1, 0x13c2, 0x13c3, 0x13c4,
        0x13c5, 0x13c6, 0x13c7, 0x13c8, 0x13c9, 0x13ca, 0x13cb,
        0x13cc, 0x13cd, 0x13ce, 0x13cf, 0x13d0, 0x13d1, 0x13d2,
        0x13d3, 0x13d4, 0x13d5, 0x13d6, 0x13d7, 0x13d8, 0x13d9,
        0x13da, 0x13db, 0x13dc, 0x13dd, 0x13de, 0x13df, 0x13e0,
        0x13e1, 0x13e2, 0x13e3, 0x13e4, 0x13e5, 0x13e6, 0x13e7,
        0x13e8, 0x13e9, 0x13ea, 0x13eb, 0x13ec, 0x13ed, 0x13ee,
        0x13ef, 0x13f0, 0x13f1, 0x13f2, 0x13f3, 0x13f4, 0x13f5,
        0x13f8, 0x13f9, 0x13fa, 0x13fb, 0x13fc, 0x13fd, 0x1d00,
        0x1d01, 0x1d02, 0x1d03, 0x1d04, 0x1d05, 0x1d06, 0x1d07,
        0x1d08, 0x1d09, 0x1d0a, 0x1d0b, 0x1d0c, 0x1d0d, 0x1d0e,
        0x1d0f, 0x1d10, 0x1d11, 0x1d12, 0x1d13, 0x1d14, 0x1d15,
        0x1d16, 0x1d17, 0x1d18, 0x1d19, 0x1d1a, 0x1d1b, 0x1d1c,
        0x1d1d, 0x1d1e, 0x1d1f, 0x1d20, 0x1d21, 0x1d22, 0x1d23,
        0x1d24, 0x1d25, 0x1d26, 0x1d27, 0x1d28, 0x1d29, 0x1d2a,
        0x1d2b, 0x1d6b, 0x1d6c, 0x1d6d, 0x1d6e, 0x1d6f, 0x1d70,
        0x1d71, 0x1d72, 0x1d73, 0x1d74, 0x1d75, 0x1d76, 0x1d77,
        0x1d79, 0x1d7a, 0x1d7b, 0x1d7c, 0x1d7d, 0x1d7e, 0x1d7f,
        0x1d80, 0x1d81, 0x1d82, 0x1d83, 0x1d84, 0x1d85, 0x1d86,
        0x1d87, 0x1d88, 0x1d89, 0x1d8a, 0x1d8b, 0x1d8c, 0x1d8d,
        0x1d8e, 0x1d8f, 0x1d90, 0x1d91, 0x1d92, 0x1d93, 0x1d94,
        0x1d95, 0x1d96, 0x1d97, 0x1d98, 0x1d99, 0x1d9a, 0x1e00,
        0x1e01, 0x1e02, 0x1e03, 0x1e04, 0x1e05, 0x1e06, 0x1e07,
        0x1e08, 0x1e09, 0x1e0a, 0x1e0b, 0x1e0c, 0x1e0d, 0x1e0e,
        0x1e0f, 0x1e10, 0x1e11, 0x1e12, 0x1e13, 0x1e14, 0x1e15,
        0x1e16, 0x1e17, 0x1e18, 0x1e19, 0x1e1a, 0x1e1b, 0x1e1c,
        0x1e1d, 0x1e1e, 0x1e1f, 0x1e20, 0x1e21, 0x1e22, 0x1e23,
        0x1e24, 0x1e25, 0x1e26, 0x1e27, 0x1e28, 0x1e29, 0x1e2a,
        0x1e2b, 0x1e2c, 0x1e2d, 0x1e2e, 0x1e2f, 0x1e30, 0x1e31,
        0x1e32, 0x1e33, 0x1e34, 0x1e35, 0x1e36, 0x1e37, 0x1e38,
        0x1e39, 0x1e3a, 0x1e3b, 0x1e3c, 0x1e3d, 0x1e3e, 0x1e3f,
        0x1e40, 0x1e41, 0x1e42, 0x1e43, 0x1e44, 0x1e45, 0x1e46,
        0x1e47, 0x1e48, 0x1e49, 0x1e4a, 0x1e4b, 0x1e4c, 0x1e4d,
        0x1e4e, 0x1e4f, 0x1e50, 0x1e51, 0x1e52, 0x1e53, 0x1e54,
        0x1e55, 0x1e56, 0x1e57, 0x1e58, 0x1e59, 0x1e5a, 0x1e5b,
        0x1e5c, 0x1e5d, 0x1e5e, 0x1e5f, 0x1e60, 0x1e61, 0x1e62,
        0x1e63, 0x1e64, 0x1e65, 0x1e66, 0x1e67, 0x1e68, 0x1e69,
        0x1e6a, 0x1e6b, 0x1e6c, 0x1e6d, 0x1e6e, 0x1e6f, 0x1e70,
        0x1e71, 0x1e72, 0x1e73, 0x1e74, 0x1e75, 0x1e76, 0x1e77,
        0x1e78, 0x1e79, 0x1e7a, 0x1e7b, 0x1e7c, 0x1e7d, 0x1e7e,
        0x1e7f, 0x1e80, 0x1e81, 0x1e82, 0x1e83, 0x1e84, 0x1e85,
        0x1e86, 0x1e87, 0x1e88, 0x1e89, 0x1e8a, 0x1e8b, 0x1e8c,
        0x1e8d, 0x1e8e, 0x1e8f, 0x1e90, 0x1e91, 0x1e92, 0x1e93,
        0x1e94, 0x1e95, 0x1e96, 0x1e97, 0x1e98, 0x1e99, 0x1e9a,
        0x1e9b, 0x1e9c, 0x1e9d, 0x1e9e, 0x1e9f, 0x1ea0, 0x1ea1,
        0x1ea2, 0x1ea3, 0x1ea4, 0x1ea5, 0x1ea6, 0x1ea7, 0x1ea8,
        0x1ea9, 0x1eaa, 0x1eab, 0x1eac, 0x1ead, 0x1eae, 0x1eaf,
        0x1eb0, 0x1eb1, 0x1eb2, 0x1eb3, 0x1eb4, 0x1eb5, 0x1eb6,
        0x1eb7, 0x1eb8, 0x1eb9, 0x1eba, 0x1ebb, 0x1ebc, 0x1ebd,
        0x1ebe, 0x1ebf, 0x1ec0, 0x1ec1, 0x1ec2, 0x1ec3, 0x1ec4,
        0x1ec5, 0x1ec6, 0x1ec7, 0x1ec8, 0x1ec9, 0x1eca, 0x1ecb,
        0x1ecc, 0x1ecd, 0x1ece, 0x1ecf, 0x1ed0, 0x1ed1, 0x1ed2,
        0x1ed3, 0x1ed4, 0x1ed5, 0x1ed6, 0x1ed7, 0x1ed8, 0x1ed9,
        0x1eda, 0x1edb, 0x1edc, 0x1edd, 0x1ede, 0x1edf, 0x1ee0,
        0x1ee1, 0x1ee2, 0x1ee3, 0x1ee4, 0x1ee5, 0x1ee6, 0x1ee7,
        0x1ee8, 0x1ee9, 0x1eea, 0x1eeb, 0x1eec, 0x1eed, 0x1eee,
        0x1eef, 0x1ef0, 0x1ef1, 0x1ef2, 0x1ef3, 0x1ef4, 0x1ef5,
        0x1ef6, 0x1ef7, 0x1ef8, 0x1ef9, 0x1efa, 0x1efb, 0x1efc,
        0x1efd, 0x1efe, 0x1eff, 0x1f00, 0x1f01, 0x1f02, 0x1f03,
        0x1f04, 0x1f05, 0x1f06, 0x1f07, 0x1f08, 0x1f09, 0x1f0a,
        0x1f0b, 0x1f0c, 0x1f0d, 0x1f0e, 0x1f0f, 0x1f10, 0x1f11,
        0x1f12, 0x1f13, 0x1f14, 0x1f15, 0x1f18, 0x1f19, 0x1f1a,
        0x1f1b, 0x1f1c, 0x1f1d, 0x1f20, 0x1f21, 0x1f22, 0x1f23,
        0x1f24, 0x1f25, 0x1f26, 0x1f27, 0x1f28, 0x1f29, 0x1f2a,
        0x1f2b, 0x1f2c, 0x1f2d, 0x1f2e, 0x1f2f, 0x1f30, 0x1f31,
        0x1f32, 0x1f33, 0x1f34, 0x1f35, 0x1f36, 0x1f37, 0x1f38,
        0x1f39, 0x1f3a, 0x1f3b, 0x1f3c, 0x1f3d, 0x1f3e, 0x1f3f,
        0x1f40, 0x1f41, 0x1f42, 0x1f43, 0x1f44, 0x1f45, 0x1f48,
        0x1f49, 0x1f4a, 0x1f4b, 0x1f4c, 0x1f4d, 0x1f50, 0x1f51,
        0x1f52, 0x1f53, 0x1f54, 0x1f55, 0x1f56, 0x1f57, 0x1f59,
        0x1f5b, 0x1f5d, 0x1f5f, 0x1f60, 0x1f61, 0x1f62, 0x1f63,
        0x1f64, 0x1f65, 0x1f66, 0x1f67, 0x1f68, 0x1f69, 0x1f6a,
        0x1f6b, 0x1f6c, 0x1f6d, 0x1f6e, 0x1f6f, 0x1f70, 0x1f71,
        0x1f72, 0x1f73, 0x1f74, 0x1f75, 0x1f76, 0x1f77, 0x1f78,
        0x1f79, 0x1f7a, 0x1f7b, 0x1f7c, 0x1f7d, 0x1f80, 0x1f81,
        0x1f82, 0x1f83, 0x1f84, 0x1f85, 0x1f86, 0x1f87, 0x1f90,
        0x1f91, 0x1f92, 0x1f93, 0x1f94, 0x1f95, 0x1f96, 0x1f97,
        0x1fa0, 0x1fa1, 0x1fa2, 0x1fa3, 0x1fa4, 0x1fa5, 0x1fa6,
        0x1fa7, 0x1fb0, 0x1fb1, 0x1fb2, 0x1fb3, 0x1fb4, 0x1fb6,
        0x1fb7, 0x1fb8, 0x1fb9, 0x1fba, 0x1fbb, 0x1fbe, 0x1fc2,
        0x1fc3, 0x1fc4, 0x1fc6, 0x1fc7, 0x1fc8, 0x1fc9, 0x1fca,
        0x1fcb, 0x1fd0, 0x1fd1, 0x1fd2, 0x1fd3, 0x1fd6, 0x1fd7,
        0x1fd8, 0x1fd9, 0x1fda, 0x1fdb, 0x1fe0, 0x1fe1, 0x1fe2,
        0x1fe3, 0x1fe4, 0x1fe5, 0x1fe6, 0x1fe7, 0x1fe8, 0x1fe9,
        0x1fea, 0x1feb, 0x1fec, 0x1ff2, 0x1ff3, 0x1ff4, 0x1ff6,
        0x1ff7, 0x1ff8, 0x1ff9, 0x1ffa, 0x1ffb, 0x2102, 0x2107,
        0x210a, 0x210b, 0x210c, 0x210d, 0x210e, 0x210f, 0x2110,
        0x2111, 0x2112, 0x2113, 0x2115, 0x2119, 0x211a, 0x211b,
        0x211c, 0x211d, 0x2124, 0x2126, 0x2128, 0x212a, 0x212b,
        0x212c, 0x212d, 0x212f, 0x2130, 0x2131, 0x2132, 0x2133,
        0x2134, 0x2139, 0x213c, 0x213d, 0x213e, 0x213f, 0x2145,
        0x2146, 0x2147, 0x2148, 0x2149, 0x214e, 0x2183, 0x2184,
        0x2c00, 0x2c01, 0x2c02, 0x2c03, 0x2c04, 0x2c05, 0x2c06,
        0x2c07, 0x2c08, 0x2c09, 0x2c0a, 0x2c0b, 0x2c0c, 0x2c0d,
        0x2c0e, 0x2c0f, 0x2c10, 0x2c11, 0x2c12, 0x2c13, 0x2c14,
        0x2c15, 0x2c16, 0x2c17, 0x2c18, 0x2c19, 0x2c1a, 0x2c1b,
        0x2c1c, 0x2c1d, 0x2c1e, 0x2c1f, 0x2c20, 0x2c21, 0x2c22,
        0x2c23, 0x2c24, 0x2c25, 0x2c26, 0x2c27, 0x2c28, 0x2c29,
        0x2c2a, 0x2c2b, 0x2c2c, 0x2c2d, 0x2c2e, 0x2c30, 0x2c31,
        0x2c32, 0x2c33, 0x2c34, 0x2c35, 0x2c36, 0x2c37, 0x2c38,
        0x2c39, 0x2c3a, 0x2c3b, 0x2c3c, 0x2c3d, 0x2c3e, 0x2c3f,
        0x2c40, 0x2c41, 0x2c42, 0x2c43, 0x2c44, 0x2c45, 0x2c46,
        0x2c47, 0x2c48, 0x2c49, 0x2c4a, 0x2c4b, 0x2c4c, 0x2c4d,
        0x2c4e, 0x2c4f, 0x2c50, 0x2c51, 0x2c52, 0x2c53, 0x2c54,
        0x2c55, 0x2c56, 0x2c57, 0x2c58, 0x2c59, 0x2c5a, 0x2c5b,
        0x2c5c, 0x2c5d, 0x2c5e, 0x2c60, 0x2c61, 0x2c62, 0x2c63,
        0x2c64, 0x2c65, 0x2c66, 0x2c67, 0x2c68, 0x2c69, 0x2c6a,
        0x2c6b, 0x2c6c, 0x2c6d, 0x2c6e, 0x2c6f, 0x2c70, 0x2c71,
        0x2c72, 0x2c73, 0x2c74, 0x2c75, 0x2c76, 0x2c77, 0x2c78,
        0x2c79, 0x2c7a, 0x2c7b, 0x2c7e, 0x2c7f, 0x2c80, 0x2c81,
        0x2c82, 0x2c83, 0x2c84, 0x2c85, 0x2c86, 0x2c87, 0x2c88,
        0x2c89, 0x2c8a, 0x2c8b, 0x2c8c, 0x2c8d, 0x2c8e, 0x2c8f,
        0x2c90, 0x2c91, 0x2c92, 0x2c93, 0x2c94, 0x2c95, 0x2c96,
        0x2c97, 0x2c98, 0x2c99, 0x2c9a, 0x2c9b, 0x2c9c, 0x2c9d,
        0x2c9e, 0x2c9f, 0x2ca0, 0x2ca1, 0x2ca2, 0x2ca3, 0x2ca4,
        0x2ca5, 0x2ca6, 0x2ca7, 0x2ca8, 0x2ca9, 0x2caa, 0x2cab,
        0x2cac, 0x2cad, 0x2cae, 0x2caf, 0x2cb0, 0x2cb1, 0x2cb2,
        0x2cb3, 0x2cb4, 0x2cb5, 0x2cb6, 0x2cb7, 0x2cb8, 0x2cb9,
        0x2cba, 0x2cbb, 0x2cbc, 0x2cbd, 0x2cbe, 0x2cbf, 0x2cc0,
        0x2cc1, 0x2cc2, 0x2cc3, 0x2cc4, 0x2cc5, 0x2cc6, 0x2cc7,
        0x2cc8, 0x2cc9, 0x2cca, 0x2ccb, 0x2ccc, 0x2ccd, 0x2cce,
        0x2ccf, 0x2cd0, 0x2cd1, 0x2cd2, 0x2cd3, 0x2cd4, 0x2cd5,
        0x2cd6, 0x2cd7, 0x2cd8, 0x2cd9, 0x2cda, 0x2cdb, 0x2cdc,
        0x2cdd, 0x2cde, 0x2cdf, 0x2ce0, 0x2ce1, 0x2ce2, 0x2ce3,
        0x2ce4, 0x2ceb, 0x2cec, 0x2ced, 0x2cee, 0x2cf2, 0x2cf3,
        0x2d00, 0x2d01, 0x2d02, 0x2d03, 0x2d04, 0x2d05, 0x2d06,
        0x2d07, 0x2d08, 0x2d09, 0x2d0a, 0x2d0b, 0x2d0c, 0x2d0d,
        0x2d0e, 0x2d0f, 0x2d10, 0x2d11, 0x2d12, 0x2d13, 0x2d14,
        0x2d15, 0x2d16, 0x2d17, 0x2d18, 0x2d19, 0x2d1a, 0x2d1b,
        0x2d1c, 0x2d1d, 0x2d1e, 0x2d1f, 0x2d20, 0x2d21, 0x2d22,
        0x2d23, 0x2d24, 0x2d25, 0x2d27, 0x2d2d, 0xa640, 0xa641,
        0xa642, 0xa643, 0xa644, 0xa645, 0xa646, 0xa647, 0xa648,
        0xa649, 0xa64a, 0xa64b, 0xa64c, 0xa64d, 0xa64e, 0xa64f,
        0xa650, 0xa651, 0xa652, 0xa653, 0xa654, 0xa655, 0xa656,
        0xa657, 0xa658, 0xa659, 0xa65a, 0xa65b, 0xa65c, 0xa65d,
        0xa65e, 0xa65f, 0xa660, 0xa661, 0xa662, 0xa663, 0xa664,
        0xa665, 0xa666, 0xa667, 0xa668, 0xa669, 0xa66a, 0xa66b,
        0xa66c, 0xa66d, 0xa680, 0xa681, 0xa682, 0xa683, 0xa684,
        0xa685, 0xa686, 0xa687, 0xa688, 0xa689, 0xa68a, 0xa68b,
        0xa68c, 0xa68d, 0xa68e, 0xa68f, 0xa690, 0xa691, 0xa692,
        0xa693, 0xa694, 0xa695, 0xa696, 0xa697, 0xa698, 0xa699,
        0xa69a, 0xa69b, 0xa722, 0xa723, 0xa724, 0xa725, 0xa726,
        0xa727, 0xa728, 0xa729, 0xa72a, 0xa72b, 0xa72c, 0xa72d,
        0xa72e, 0xa72f, 0xa730, 0xa731, 0xa732, 0xa733, 0xa734,
        0xa735, 0xa736, 0xa737, 0xa738, 0xa739, 0xa73a, 0xa73b,
        0xa73c, 0xa73d, 0xa73e, 0xa73f, 0xa740, 0xa741, 0xa742,
        0xa743, 0xa744, 0xa745, 0xa746, 0xa747, 0xa748, 0xa749,
        0xa74a, 0xa74b, 0xa74c, 0xa74d, 0xa74e, 0xa74f, 0xa750,
        0xa751, 0xa752, 0xa753, 0xa754, 0xa755, 0xa756, 0xa757,
        0xa758, 0xa759, 0xa75a, 0xa75b, 0xa75c, 0xa75d, 0xa75e,
        0xa75f, 0xa760, 0xa761, 0xa762, 0xa763, 0xa764, 0xa765,
        0xa766, 0xa767, 0xa768, 0xa769, 0xa76a, 0xa76b, 0xa76c,
        0xa76d, 0xa76e, 0xa76f, 0xa771, 0xa772, 0xa773, 0xa774,
        0xa775, 0xa776, 0xa777, 0xa778, 0xa779, 0xa77a, 0xa77b,
        0xa77c, 0xa77d, 0xa77e, 0xa77f, 0xa780, 0xa781, 0xa782,
        0xa783, 0xa784, 0xa785, 0xa786, 0xa787, 0xa78b, 0xa78c,
        0xa78d, 0xa78e, 0xa790, 0xa791, 0xa792, 0xa793, 0xa794,
        0xa795, 0xa796, 0xa797, 0xa798, 0xa799, 0xa79a, 0xa79b,
        0xa79c, 0xa79d, 0xa79e, 0xa79f, 0xa7a0, 0xa7a1, 0xa7a2,
        0xa7a3, 0xa7a4, 0xa7a5, 0xa7a6, 0xa7a7, 0xa7a8, 0xa7a9,
        0xa7aa, 0xa7ab, 0xa7ac, 0xa7ad, 0xa7b0, 0xa7b1, 0xa7b2,
        0xa7b3, 0xa7b4, 0xa7b5, 0xa7b6, 0xa7b7, 0xa7fa, 0xab30,
        0xab31, 0xab32, 0xab33, 0xab34, 0xab35, 0xab36, 0xab37,
        0xab38, 0xab39, 0xab3a, 0xab3b, 0xab3c, 0xab3d, 0xab3e,
        0xab3f, 0xab40, 0xab41, 0xab42, 0xab43, 0xab44, 0xab45,
        0xab46, 0xab47, 0xab48, 0xab49, 0xab4a, 0xab4b, 0xab4c,
        0xab4d, 0xab4e, 0xab4f, 0xab50, 0xab51, 0xab52, 0xab53,
        0xab54, 0xab55, 0xab56, 0xab57, 0xab58, 0xab59, 0xab5a,
        0xab60, 0xab61, 0xab62, 0xab63, 0xab64, 0xab65, 0xab70,
        0xab71, 0xab72, 0xab73, 0xab74, 0xab75, 0xab76, 0xab77,
        0xab78, 0xab79, 0xab7a, 0xab7b, 0xab7c, 0xab7d, 0xab7e,
        0xab7f, 0xab80, 0xab81, 0xab82, 0xab83, 0xab84, 0xab85,
        0xab86, 0xab87, 0xab88, 0xab89, 0xab8a, 0xab8b, 0xab8c,
        0xab8d, 0xab8e, 0xab8f, 0xab90, 0xab91, 0xab92, 0xab93,
        0xab94, 0xab95, 0xab96, 0xab97, 0xab98, 0xab99, 0xab9a,
        0xab9b, 0xab9c, 0xab9d, 0xab9e, 0xab9f, 0xaba0, 0xaba1,
        0xaba2, 0xaba3, 0xaba4, 0xaba5, 0xaba6, 0xaba7, 0xaba8,
        0xaba9, 0xabaa, 0xabab, 0xabac, 0xabad, 0xabae, 0xabaf,
        0xabb0, 0xabb1, 0xabb2, 0xabb3, 0xabb4, 0xabb5, 0xabb6,
        0xabb7, 0xabb8, 0xabb9, 0xabba, 0xabbb, 0xabbc, 0xabbd,
        0xabbe, 0xabbf, 0xfb00, 0xfb01, 0xfb02, 0xfb03, 0xfb04,
        0xfb05, 0xfb06, 0xfb13, 0xfb14, 0xfb15, 0xfb16, 0xfb17,
        0xff21, 0xff22, 0xff23, 0xff24, 0xff25, 0xff26, 0xff27,
        0xff28, 0xff29, 0xff2a, 0xff2b, 0xff2c, 0xff2d, 0xff2e,
        0xff2f, 0xff30, 0xff31, 0xff32, 0xff33, 0xff34, 0xff35,
        0xff36, 0xff37, 0xff38, 0xff39, 0xff3a, 0xff41, 0xff42,
        0xff43, 0xff44, 0xff45, 0xff46, 0xff47, 0xff48, 0xff49,
        0xff4a, 0xff4b, 0xff4c, 0xff4d, 0xff4e, 0xff4f, 0xff50,
        0xff51, 0xff52, 0xff53, 0xff54, 0xff55, 0xff56, 0xff57,
        0xff58, 0xff59, 0xff5a, 0x10400, 0x10401, 0x10402, 0x10403,
        0x10404, 0x10405, 0x10406, 0x10407, 0x10408, 0x10409, 0x1040a,
        0x1040b, 0x1040c, 0x1040d, 0x1040e, 0x1040f, 0x10410, 0x10411,
        0x10412, 0x10413, 0x10414, 0x10415, 0x10416, 0x10417, 0x10418,
        0x10419, 0x1041a, 0x1041b, 0x1041c, 0x1041d, 0x1041e, 0x1041f,
        0x10420, 0x10421, 0x10422, 0x10423, 0x10424, 0x10425, 0x10426,
        0x10427, 0x10428, 0x10429, 0x1042a, 0x1042b, 0x1042c, 0x1042d,
        0x1042e, 0x1042f, 0x10430, 0x10431, 0x10432, 0x10433, 0x10434,
        0x10435, 0x10436, 0x10437, 0x10438, 0x10439, 0x1043a, 0x1043b,
        0x1043c, 0x1043d, 0x1043e, 0x1043f, 0x10440, 0x10441, 0x10442,
        0x10443, 0x10444, 0x10445, 0x10446, 0x10447, 0x10448, 0x10449,
        0x1044a, 0x1044b, 0x1044c, 0x1044d, 0x1044e, 0x1044f, 0x10c80,
        0x10c81, 0x10c82, 0x10c83, 0x10c84, 0x10c85, 0x10c86, 0x10c87,
        0x10c88, 0x10c89, 0x10c8a, 0x10c8b, 0x10c8c, 0x10c8d, 0x10c8e,
        0x10c8f, 0x10c90, 0x10c91, 0x10c92, 0x10c93, 0x10c94, 0x10c95,
        0x10c96, 0x10c97, 0x10c98, 0x10c99, 0x10c9a, 0x10c9b, 0x10c9c,
        0x10c9d, 0x10c9e, 0x10c9f, 0x10ca0, 0x10ca1, 0x10ca2, 0x10ca3,
        0x10ca4, 0x10ca5, 0x10ca6, 0x10ca7, 0x10ca8, 0x10ca9, 0x10caa,
        0x10cab, 0x10cac, 0x10cad, 0x10cae, 0x10caf, 0x10cb0, 0x10cb1,
        0x10cb2, 0x10cc0, 0x10cc1, 0x10cc2, 0x10cc3, 0x10cc4, 0x10cc5,
        0x10cc6, 0x10cc7, 0x10cc8, 0x10cc9, 0x10cca, 0x10ccb, 0x10ccc,
        0x10ccd, 0x10cce, 0x10ccf, 0x10cd0, 0x10cd1, 0x10cd2, 0x10cd3,
        0x10cd4, 0x10cd5, 0x10cd6, 0x10cd7, 0x10cd8, 0x10cd9, 0x10cda,
        0x10cdb, 0x10cdc, 0x10cdd, 0x10cde, 0x10cdf, 0x10ce0, 0x10ce1,
        0x10ce2, 0x10ce3, 0x10ce4, 0x10ce5, 0x10ce6, 0x10ce7, 0x10ce8,
        0x10ce9, 0x10cea, 0x10ceb, 0x10cec, 0x10ced, 0x10cee, 0x10cef,
        0x10cf0, 0x10cf1, 0x10cf2, 0x118a0, 0x118a1, 0x118a2, 0x118a3,
        0x118a4, 0x118a5, 0x118a6, 0x118a7, 0x118a8, 0x118a9, 0x118aa,
        0x118ab, 0x118ac, 0x118ad, 0x118ae, 0x118af, 0x118b0, 0x118b1,
        0x118b2, 0x118b3, 0x118b4, 0x118b5, 0x118b6, 0x118b7, 0x118b8,
        0x118b9, 0x118ba, 0x118bb, 0x118bc, 0x118bd, 0x118be, 0x118bf,
        0x118c0, 0x118c1, 0x118c2, 0x118c3, 0x118c4, 0x118c5, 0x118c6,
        0x118c7, 0x118c8, 0x118c9, 0x118ca, 0x118cb, 0x118cc, 0x118cd,
        0x118ce, 0x118cf, 0x118d0, 0x118d1, 0x118d2, 0x118d3, 0x118d4,
        0x118d5, 0x118d6, 0x118d7, 0x118d8, 0x118d9, 0x118da, 0x118db,
        0x118dc, 0x118dd, 0x118de, 0x118df, 0x1d400, 0x1d401, 0x1d402,
        0x1d403, 0x1d404, 0x1d405, 0x1d406, 0x1d407, 0x1d408, 0x1d409,
        0x1d40a, 0x1d40b, 0x1d40c, 0x1d40d, 0x1d40e, 0x1d40f, 0x1d410,
        0x1d411, 0x1d412, 0x1d413, 0x1d414, 0x1d415, 0x1d416, 0x1d417,
        0x1d418, 0x1d419, 0x1d41a, 0x1d41b, 0x1d41c, 0x1d41d, 0x1d41e,
        0x1d41f, 0x1d420, 0x1d421, 0x1d422, 0x1d423, 0x1d424, 0x1d425,
        0x1d426, 0x1d427, 0x1d428, 0x1d429, 0x1d42a, 0x1d42b, 0x1d42c,
        0x1d42d, 0x1d42e, 0x1d42f, 0x1d430, 0x1d431, 0x1d432, 0x1d433,
        0x1d434, 0x1d435, 0x1d436, 0x1d437, 0x1d438, 0x1d439, 0x1d43a,
        0x1d43b, 0x1d43c, 0x1d43d, 0x1d43e, 0x1d43f, 0x1d440, 0x1d441,
        0x1d442, 0x1d443, 0x1d444, 0x1d445, 0x1d446, 0x1d447, 0x1d448,
        0x1d449, 0x1d44a, 0x1d44b, 0x1d44c, 0x1d44d, 0x1d44e, 0x1d44f,
        0x1d450, 0x1d451, 0x1d452, 0x1d453, 0x1d454, 0x1d456, 0x1d457,
        0x1d458, 0x1d459, 0x1d45a, 0x1d45b, 0x1d45c, 0x1d45d, 0x1d45e,
        0x1d45f, 0x1d460, 0x1d461, 0x1d462, 0x1d463, 0x1d464, 0x1d465,
        0x1d466, 0x1d467, 0x1d468, 0x1d469, 0x1d46a, 0x1d46b, 0x1d46c,
        0x1d46d, 0x1d46e, 0x1d46f, 0x1d470, 0x1d471, 0x1d472, 0x1d473,
        0x1d474, 0x1d475, 0x1d476, 0x1d477, 0x1d478, 0x1d479, 0x1d47a,
        0x1d47b, 0x1d47c, 0x1d47d, 0x1d47e, 0x1d47f, 0x1d480, 0x1d481,
        0x1d482, 0x1d483, 0x1d484, 0x1d485, 0x1d486, 0x1d487, 0x1d488,
        0x1d489, 0x1d48a, 0x1d48b, 0x1d48c, 0x1d48d, 0x1d48e, 0x1d48f,
        0x1d490, 0x1d491, 0x1d492, 0x1d493, 0x1d494, 0x1d495, 0x1d496,
        0x1d497, 0x1d498, 0x1d499, 0x1d49a, 0x1d49b, 0x1d49c, 0x1d49e,
        0x1d49f, 0x1d4a2, 0x1d4a5, 0x1d4a6, 0x1d4a9, 0x1d4aa, 0x1d4ab,
        0x1d4ac, 0x1d4ae, 0x1d4af, 0x1d4b0, 0x1d4b1, 0x1d4b2, 0x1d4b3,
        0x1d4b4, 0x1d4b5, 0x1d4b6, 0x1d4b7, 0x1d4b8, 0x1d4b9, 0x1d4bb,
        0x1d4bd, 0x1d4be, 0x1d4bf, 0x1d4c0, 0x1d4c1, 0x1d4c2, 0x1d4c3,
        0x1d4c5, 0x1d4c6, 0x1d4c7, 0x1d4c8, 0x1d4c9, 0x1d4ca, 0x1d4cb,
        0x1d4cc, 0x1d4cd, 0x1d4ce, 0x1d4cf, 0x1d4d0, 0x1d4d1, 0x1d4d2,
        0x1d4d3, 0x1d4d4, 0x1d4d5, 0x1d4d6, 0x1d4d7, 0x1d4d8, 0x1d4d9,
        0x1d4da, 0x1d4db, 0x1d4dc, 0x1d4dd, 0x1d4de, 0x1d4df, 0x1d4e0,
        0x1d4e1, 0x1d4e2, 0x1d4e3, 0x1d4e4, 0x1d4e5, 0x1d4e6, 0x1d4e7,
        0x1d4e8, 0x1d4e9, 0x1d4ea, 0x1d4eb, 0x1d4ec, 0x1d4ed, 0x1d4ee,
        0x1d4ef, 0x1d4f0, 0x1d4f1, 0x1d4f2, 0x1d4f3, 0x1d4f4, 0x1d4f5,
        0x1d4f6, 0x1d4f7, 0x1d4f8, 0x1d4f9, 0x1d4fa, 0x1d4fb, 0x1d4fc,
        0x1d4fd, 0x1d4fe, 0x1d4ff, 0x1d500, 0x1d501, 0x1d502, 0x1d503,
        0x1d504, 0x1d505, 0x1d507, 0x1d508, 0x1d509, 0x1d50a, 0x1d50d,
        0x1d50e, 0x1d50f, 0x1d510, 0x1d511, 0x1d512, 0x1d513, 0x1d514,
        0x1d516, 0x1d517, 0x1d518, 0x1d519, 0x1d51a, 0x1d51b, 0x1d51c,
        0x1d51e, 0x1d51f, 0x1d520, 0x1d521, 0x1d522, 0x1d523, 0x1d524,
        0x1d525, 0x1d526, 0x1d527, 0x1d528, 0x1d529, 0x1d52a, 0x1d52b,
        0x1d52c, 0x1d52d, 0x1d52e, 0x1d52f, 0x1d530, 0x1d531, 0x1d532,
        0x1d533, 0x1d534, 0x1d535, 0x1d536, 0x1d537, 0x1d538, 0x1d539,
        0x1d53b, 0x1d53c, 0x1d53d, 0x1d53e, 0x1d540, 0x1d541, 0x1d542,
        0x1d543, 0x1d544, 0x1d546, 0x1d54a, 0x1d54b, 0x1d54c, 0x1d54d,
        0x1d54e, 0x1d54f, 0x1d550, 0x1d552, 0x1d553, 0x1d554, 0x1d555,
        0x1d556, 0x1d557, 0x1d558, 0x1d559, 0x1d55a, 0x1d55b, 0x1d55c,
        0x1d55d, 0x1d55e, 0x1d55f, 0x1d560, 0x1d561, 0x1d562, 0x1d563,
        0x1d564, 0x1d565, 0x1d566, 0x1d567, 0x1d568, 0x1d569, 0x1d56a,
        0x1d56b, 0x1d56c, 0x1d56d, 0x1d56e, 0x1d56f, 0x1d570, 0x1d571,
        0x1d572, 0x1d573, 0x1d574, 0x1d575, 0x1d576, 0x1d577, 0x1d578,
        0x1d579, 0x1d57a, 0x1d57b, 0x1d57c, 0x1d57d, 0x1d57e, 0x1d57f,
        0x1d580, 0x1d581, 0x1d582, 0x1d583, 0x1d584, 0x1d585, 0x1d586,
        0x1d587, 0x1d588, 0x1d589, 0x1d58a, 0x1d58b, 0x1d58c, 0x1d58d,
        0x1d58e, 0x1d58f, 0x1d590, 0x1d591, 0x1d592, 0x1d593, 0x1d594,
        0x1d595, 0x1d596, 0x1d597, 0x1d598, 0x1d599, 0x1d59a, 0x1d59b,
        0x1d59c, 0x1d59d, 0x1d59e, 0x1d59f, 0x1d5a0, 0x1d5a1, 0x1d5a2,
        0x1d5a3, 0x1d5a4, 0x1d5a5, 0x1d5a6, 0x1d5a7, 0x1d5a8, 0x1d5a9,
        0x1d5aa, 0x1d5ab, 0x1d5ac, 0x1d5ad, 0x1d5ae, 0x1d5af, 0x1d5b0,
        0x1d5b1, 0x1d5b2, 0x1d5b3, 0x1d5b4, 0x1d5b5, 0x1d5b6, 0x1d5b7,
        0x1d5b8, 0x1d5b9, 0x1d5ba, 0x1d5bb, 0x1d5bc, 0x1d5bd, 0x1d5be,
        0x1d5bf, 0x1d5c0, 0x1d5c1, 0x1d5c2, 0x1d5c3, 0x1d5c4, 0x1d5c5,
        0x1d5c6, 0x1d5c7, 0x1d5c8, 0x1d5c9, 0x1d5ca, 0x1d5cb, 0x1d5cc,
        0x1d5cd, 0x1d5ce, 0x1d5cf, 0x1d5d0, 0x1d5d1, 0x1d5d2, 0x1d5d3,
        0x1d5d4, 0x1d5d5, 0x1d5d6, 0x1d5d7, 0x1d5d8, 0x1d5d9, 0x1d5da,
        0x1d5db, 0x1d5dc, 0x1d5dd, 0x1d5de, 0x1d5df, 0x1d5e0, 0x1d5e1,
        0x1d5e2, 0x1d5e3, 0x1d5e4, 0x1d5e5, 0x1d5e6, 0x1d5e7, 0x1d5e8,
        0x1d5e9, 0x1d5ea, 0x1d5eb, 0x1d5ec, 0x1d5ed, 0x1d5ee, 0x1d5ef,
        0x1d5f0, 0x1d5f1, 0x1d5f2, 0x1d5f3, 0x1d5f4, 0x1d5f5, 0x1d5f6,
        0x1d5f7, 0x1d5f8, 0x1d5f9, 0x1d5fa, 0x1d5fb, 0x1d5fc, 0x1d5fd,
        0x1d5fe, 0x1d5ff, 0x1d600, 0x1d601, 0x1d602, 0x1d603, 0x1d604,
        0x1d605, 0x1d606, 0x1d607, 0x1d608, 0x1d609, 0x1d60a, 0x1d60b,
        0x1d60c, 0x1d60d, 0x1d60e, 0x1d60f, 0x1d610, 0x1d611, 0x1d612,
        0x1d613, 0x1d614, 0x1d615, 0x1d616, 0x1d617, 0x1d618, 0x1d619,
        0x1d61a, 0x1d61b, 0x1d61c, 0x1d61d, 0x1d61e, 0x1d61f, 0x1d620,
        0x1d621, 0x1d622, 0x1d623, 0x1d624, 0x1d625, 0x1d626, 0x1d627,
        0x1d628, 0x1d629, 0x1d62a, 0x1d62b, 0x1d62c, 0x1d62d, 0x1d62e,
        0x1d62f, 0x1d630, 0x1d631, 0x1d632, 0x1d633, 0x1d634, 0x1d635,
        0x1d636, 0x1d637, 0x1d638, 0x1d639, 0x1d63a, 0x1d63b, 0x1d63c,
        0x1d63d, 0x1d63e, 0x1d63f, 0x1d640, 0x1d641, 0x1d642, 0x1d643,
        0x1d644, 0x1d645, 0x1d646, 0x1d647, 0x1d648, 0x1d649, 0x1d64a,
        0x1d64b, 0x1d64c, 0x1d64d, 0x1d64e, 0x1d64f, 0x1d650, 0x1d651,
        0x1d652, 0x1d653, 0x1d654, 0x1d655, 0x1d656, 0x1d657, 0x1d658,
        0x1d659, 0x1d65a, 0x1d65b, 0x1d65c, 0x1d65d, 0x1d65e, 0x1d65f,
        0x1d660, 0x1d661, 0x1d662, 0x1d663, 0x1d664, 0x1d665, 0x1d666,
        0x1d667, 0x1d668, 0x1d669, 0x1d66a, 0x1d66b, 0x1d66c, 0x1d66d,
        0x1d66e, 0x1d66f, 0x1d670, 0x1d671, 0x1d672, 0x1d673, 0x1d674,
        0x1d675, 0x1d676, 0x1d677, 0x1d678, 0x1d679, 0x1d67a, 0x1d67b,
        0x1d67c, 0x1d67d, 0x1d67e, 0x1d67f, 0x1d680, 0x1d681, 0x1d682,
        0x1d683, 0x1d684, 0x1d685, 0x1d686, 0x1d687, 0x1d688, 0x1d689,
        0x1d68a, 0x1d68b, 0x1d68c, 0x1d68d, 0x1d68e, 0x1d68f, 0x1d690,
        0x1d691, 0x1d692, 0x1d693, 0x1d694, 0x1d695, 0x1d696, 0x1d697,
        0x1d698, 0x1d699, 0x1d69a, 0x1d69b, 0x1d69c, 0x1d69d, 0x1d69e,
        0x1d69f, 0x1d6a0, 0x1d6a1, 0x1d6a2, 0x1d6a3, 0x1d6a4, 0x1d6a5,
        0x1d6a8, 0x1d6a9, 0x1d6aa, 0x1d6ab, 0x1d6ac, 0x1d6ad, 0x1d6ae,
        0x1d6af, 0x1d6b0, 0x1d6b1, 0x1d6b2, 0x1d6b3, 0x1d6b4, 0x1d6b5,
        0x1d6b6, 0x1d6b7, 0x1d6b8, 0x1d6b9, 0x1d6ba, 0x1d6bb, 0x1d6bc,
        0x1d6bd, 0x1d6be, 0x1d6bf, 0x1d6c0, 0x1d6c2, 0x1d6c3, 0x1d6c4,
        0x1d6c5, 0x1d6c6, 0x1d6c7, 0x1d6c8, 0x1d6c9, 0x1d6ca, 0x1d6cb,
        0x1d6cc, 0x1d6cd, 0x1d6ce, 0x1d6cf, 0x1d6d0, 0x1d6d1, 0x1d6d2,
        0x1d6d3, 0x1d6d4, 0x1d6d5, 0x1d6d6, 0x1d6d7, 0x1d6d8, 0x1d6d9,
        0x1d6da, 0x1d6dc, 0x1d6dd, 0x1d6de, 0x1d6df, 0x1d6e0, 0x1d6e1,
        0x1d6e2, 0x1d6e3, 0x1d6e4, 0x1d6e5, 0x1d6e6, 0x1d6e7, 0x1d6e8,
        0x1d6e9, 0x1d6ea, 0x1d6eb, 0x1d6ec, 0x1d6ed, 0x1d6ee, 0x1d6ef,
        0x1d6f0, 0x1d6f1, 0x1d6f2, 0x1d6f3, 0x1d6f4, 0x1d6f5, 0x1d6f6,
        0x1d6f7, 0x1d6f8, 0x1d6f9, 0x1d6fa, 0x1d6fc, 0x1d6fd, 0x1d6fe,
        0x1d6ff, 0x1d700, 0x1d701, 0x1d702, 0x1d703, 0x1d704, 0x1d705,
        0x1d706, 0x1d707, 0x1d708, 0x1d709, 0x1d70a, 0x1d70b, 0x1d70c,
        0x1d70d, 0x1d70e, 0x1d70f, 0x1d710, 0x1d711, 0x1d712, 0x1d713,
        0x1d714, 0x1d716, 0x1d717, 0x1d718, 0x1d719, 0x1d71a, 0x1d71b,
        0x1d71c, 0x1d71d, 0x1d71e, 0x1d71f, 0x1d720, 0x1d721, 0x1d722,
        0x1d723, 0x1d724, 0x1d725, 0x1d726, 0x1d727, 0x1d728, 0x1d729,
        0x1d72a, 0x1d72b, 0x1d72c, 0x1d72d, 0x1d72e, 0x1d72f, 0x1d730,
        0x1d731, 0x1d732, 0x1d733, 0x1d734, 0x1d736, 0x1d737, 0x1d738,
        0x1d739, 0x1d73a, 0x1d73b, 0x1d73c, 0x1d73d, 0x1d73e, 0x1d73f,
        0x1d740, 0x1d741, 0x1d742, 0x1d743, 0x1d744, 0x1d745, 0x1d746,
        0x1d747, 0x1d748, 0x1d749, 0x1d74a, 0x1d74b, 0x1d74c, 0x1d74d,
        0x1d74e, 0x1d750, 0x1d751, 0x1d752, 0x1d753, 0x1d754, 0x1d755,
        0x1d756, 0x1d757, 0x1d758, 0x1d759, 0x1d75a, 0x1d75b, 0x1d75c,
        0x1d75d, 0x1d75e, 0x1d75f, 0x1d760, 0x1d761, 0x1d762, 0x1d763,
        0x1d764, 0x1d765, 0x1d766, 0x1d767, 0x1d768, 0x1d769, 0x1d76a,
        0x1d76b, 0x1d76c, 0x1d76d, 0x1d76e, 0x1d770, 0x1d771, 0x1d772,
        0x1d773, 0x1d774, 0x1d775, 0x1d776, 0x1d777, 0x1d778, 0x1d779,
        0x1d77a, 0x1d77b, 0x1d77c, 0x1d77d, 0x1d77e, 0x1d77f, 0x1d780,
        0x1d781, 0x1d782, 0x1d783, 0x1d784, 0x1d785, 0x1d786, 0x1d787,
        0x1d788, 0x1d78a, 0x1d78b, 0x1d78c, 0x1d78d, 0x1d78e, 0x1d78f,
        0x1d790, 0x1d791, 0x1d792, 0x1d793, 0x1d794, 0x1d795, 0x1d796,
        0x1d797, 0x1d798, 0x1d799, 0x1d79a, 0x1d79b, 0x1d79c, 0x1d79d,
        0x1d79e, 0x1d79f, 0x1d7a0, 0x1d7a1, 0x1d7a2, 0x1d7a3, 0x1d7a4,
        0x1d7a5, 0x1d7a6, 0x1d7a7, 0x1d7a8, 0x1d7aa, 0x1d7ab, 0x1d7ac,
        0x1d7ad, 0x1d7ae, 0x1d7af, 0x1d7b0, 0x1d7b1, 0x1d7b2, 0x1d7b3,
        0x1d7b4, 0x1d7b5, 0x1d7b6, 0x1d7b7, 0x1d7b8, 0x1d7b9, 0x1d7ba,
        0x1d7bb, 0x1d7bc, 0x1d7bd, 0x1d7be, 0x1d7bf, 0x1d7c0, 0x1d7c1,
        0x1d7c2, 0x1d7c4, 0x1d7c5, 0x1d7c6, 0x1d7c7, 0x1d7c8, 0x1d7c9,
        0x1d7ca, 0x1d7cb
    };
}

bool IsSymbolic(LispChar c)
{
    for (const char *ptr = symbolics; *ptr; ++ptr)
        if (*ptr == c)
            return true;

    return false;
}

// utility functions
#ifdef YACAS_UINT32_T_IN_GLOBAL_NAMESPACE
bool IsAlpha(uint32_t c)
#else
bool IsAlpha(std::uint32_t c)
#endif
{
    return letters.count(c) || c == '\'';
}

#ifdef YACAS_UINT32_T_IN_GLOBAL_NAMESPACE
bool IsAlNum(uint32_t c)
#else
bool IsAlNum(std::uint32_t c)
#endif
{
  return IsAlpha(c) || std::isdigit(c);
}


const LispString* LispTokenizer::NextToken(LispInput& aInput,
                                           LispHashTable& aHashTable)
{
#ifdef YACAS_UINT32_T_IN_GLOBAL_NAMESPACE
	uint32_t c;
#else
	std::uint32_t c;
#endif

    // skip whitespaces and comments
    for (;;) {
        // End of stream: return empty string
        if (aInput.EndOfStream())
            return aHashTable.LookUp("");

        c = aInput.Next();
    
        if (std::isspace(c))
            continue;

        // parse comments
        if (c == '/' && aInput.Peek() == '*') {
            aInput.Next();
            for (;;) {
                while (aInput.Next() != '*' && !aInput.EndOfStream())
                    ;

                if (aInput.EndOfStream())
                    throw LispErrCommentToEndOfFile();

                if (aInput.Peek() == '/') {
                    aInput.Next();
                    break;
                }
            }

            continue;
        }

        if (c == '/' && aInput.Peek() == '/') {
            aInput.Next();
            while (aInput.Next() != '\n' && !aInput.EndOfStream())
                ;
            continue;
        }

        break;
    }

    // parse brackets
    if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']')
        return aHashTable.LookUp(std::string(1, c));

    // percent
    if (c == '%')
        return aHashTable.LookUp(std::string(1, c));

    // comma and semicolon
    if (c == ',' || c == ';')
        return aHashTable.LookUp(std::string(1, c));

    // parse . or ..
    if (c == '.' && !std::isdigit(aInput.Peek())) {
        std::string token;
        token.push_back(c);
        while (aInput.Peek() == '.')
            token.push_back(aInput.Next());
        return aHashTable.LookUp(token);
    }

    // parse literal strings
    if (c == '\"') {
        std::string str;
        utf8::append(c, std::back_inserter(str));
        while (aInput.Peek() != '\"') {
            if (aInput.Peek() == '\\') {
                aInput.Next();

                if (aInput.EndOfStream())
                    throw LispErrParsingInput();
            }
            utf8::append(aInput.Next(), std::back_inserter(str));

            if (aInput.EndOfStream())
                throw LispErrParsingInput();
        }
        utf8::append(aInput.Next(), std::back_inserter(str));
        return aHashTable.LookUp(str);
    }

    // parse atoms
    if (IsAlpha(c)) {
        std::string atom;
        utf8::append(c, std::back_inserter(atom));
        while (IsAlNum(aInput.Peek()))
            utf8::append(aInput.Next(), std::back_inserter(atom));
        return aHashTable.LookUp(atom);
    }

    // parse operators
    if (IsSymbolic(c)) {
        std::string op;
        op.push_back(c);
        while (IsSymbolic(aInput.Peek()))
            op.push_back(aInput.Next());
        return aHashTable.LookUp(op);
    }

    // parse subscripts
    if (c == '_') {
        std::string token;
        utf8::append(c, std::back_inserter(token));
        while (aInput.Peek() == '_')
            utf8::append(aInput.Next(), std::back_inserter(token));
        return aHashTable.LookUp(token);
    }

    // parse numbers
    if (std::isdigit(c) || c == '.') {
        std::string number;
        number.push_back(c);

        while (std::isdigit(aInput.Peek()))
            number.push_back(aInput.Next());

        if (aInput.Peek() == '.') {
            number.push_back(aInput.Next());
            while (std::isdigit(aInput.Peek()))
                number.push_back(aInput.Next());
        }

        if (aInput.Peek() == 'e' || aInput.Peek() == 'E') {
            number.push_back(aInput.Next());
            if (aInput.Peek() == '-' || aInput.Peek() == '+')
                number.push_back(aInput.Next());
            while (std::isdigit(aInput.Peek()))
                number.push_back(aInput.Next());
        }

        return aHashTable.LookUp(number);
    }

    throw InvalidToken();
}
