//
// Copyright (C) 2019-2019 markhc
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

#include <string_view>

constexpr std::string_view game_samples[] = {
    // Game 1
    "1. e4 d5 2. exd5 Qxd5 3. Nc3 Qe5+ 4. Be2 c6 5. Nf3 Qc7 6. O-O Nf6 7. d4 Bg4 8. Bg5 Bxf3 9. "
    "Bxf3 e6 10. Bh4 Bd6 11. Bg3 Nbd7 12. Re1 Bxg3 13. hxg3 O-O-O 14. Ne4 h5 15. c3 Nxe4 16. Bxe4 "
    "h4 17. g4 h3 18. g3 Nf6 19. Bh1 h2+ 20. Kg2 Kb8 21. Qf3 c5 22. Qf4 Qxf4 23. gxf4 cxd4 24. "
    "cxd4 Nxg4 25. Kf3 f5 26. Rxe6 Rxd4 27. Rc1 Rd2 28. Kg3 Nxf2 29. Re7 Rxb2 30. Rcc7 Nxh1+ 31. "
    "Kf3 Nf2 32. Ke3 h1=Q 0-1",
    // Game 2
    "1. e4 g6 2. Nf3 Bg7 3. d4 d6 4. c3 Nd7 5. Be2 c5 6. O-O Ngf6 7. Be3 Nxe4 8. dxc5 Nexc5 9. "
    "Bxc5 Nxc5 10. Nbd2 O-O 11. Nb3 b6 12. Nxc5 bxc5 13. Re1 Rb8 14. Qc2 Bf5 15. Bd3 Bxd3 16. Qxd3 "
    "Rxb2 17. Reb1 Qb6 18. Rxb2 Qxb2 19. Rb1 Qxc3 20. Qxc3 Bxc3 21. Rb7 Bf6 22. Rxa7 c4 23. Ne1 c3 "
    "24. Rc7 Rb8 25. h3 Rb2 26. a4 Bd4 27. Rc8+ Kg7 28. Ra8 Bxf2+ 29. Kf1 Bg3 30. Re8 Rf2+ 31. Kg1 "
    "Re2 32. Nf3 c2 33. Nd4 Re1# 0-1",
    // Game 3
    "1. g3 Nf6 2. e3 e6 3. Bg2 d5 4. Ne2 c5 5. O-O Nc6 6. d4 Be7 7. dxc5 O-O 8. c4 Bxc5 9. cxd5 "
    "exd5 10. Nbc3 Bg4 11. h3 Bxe2 12. Qxe2 d4 13. Rd1 Qe7 14. exd4 Qxe2 15. Nxe2 Rad8 16. d5 Nb4 "
    "17. Bg5 Rfe8 18. Nc3 Nc2 19. Rac1 Nd4 20. Kf1 Bb6 21. Bxf6 gxf6 22. Ne4 Nf5 23. Nxf6+ Kg7 24. "
    "Nxe8+ Rxe8 25. d6 Rd8 26. d7 Ne7 27. Bxb7 Nc6 28. Rxc6 1-0",
    // Game 4
    "1. e3 d5 2. Be2 e6 3. Nf3 f5 4. O-O Nf6 5. d3 c6 6. c4 Bd6 7. cxd5 O-O 8. Nc3 exd5 9. b3 Nbd7 "
    "10. Bb2 Qe7 11. Nd4 Be5 12. Nxf5 Qe6 13. e4 dxe4 14. dxe4 Nxe4 15. Bc4 Qxc4 16. Ne7+ Kh8 17. "
    "bxc4 Ndc5 18. Nxe4 Bxb2 19. Nxc5 Bh3 20. Rb1 Bf6 21. gxh3 Bxe7 22. Nd7 Rf6 23. Nxf6 Bxf6 24. "
    "Rxb7 Rf8 25. Qd6 Bh4 26. Qxf8# 1-0",
    // Game 5
    "1. Nf3 d6 2. g3 g6 3. Bg2 Bg7 4. O-O Nf6 5. d4 O-O 6. c4 c6 7. Nc3 Nbd7 8. e4 e5 9. dxe5 dxe5 "
    "10. Qc2 Qc7 11. Rd1 Nc5 12. Be3 Ne6 13. c5 Ng4 14. Na4 Nxe3 15. fxe3 b5 16. cxb6 axb6 17. a3 "
    "Bb7 18. Nc3 c5 19. Nd5 Bxd5 20. exd5 Nd8 21. d6 Qd7 22. Ng5 Rc8 23. Bh3 f5 24. e4 h6 25. Nf3 "
    "Nf7 26. Nh4 Qe6 27. exf5 gxf5 28. Bxf5 Qf6 29. Bh7+ Kh8 30. Ng6+ Qxg6 31. Bxg6 Rcd8 32. d7 "
    "Ng5 33. h4 Ne6 34. Rd6 Nd4 35. Qe4 Rf6 36. Rxf6 Bxf6 37. Rf1 Bg7 38. Be8 Ne6 39. Qg6 Nf8 40. "
    "Rxf8+ Bxf8 41. Qf6+ Bg7 42. Qxd8 Kh7 43. Qe7 Kg8 44. d8=Q Kh7 45. h5 e4 46. Bg6# 1-0",
    // Game 6
    "1. d4 g6 2. c4 Bg7 3. Nf3 d6 4. g3 Nf6 5. Bg2 O-O 6. O-O c6 7. Nc3 Nbd7 8. e4 Re8 9. Qc2 e5 "
    "10. dxe5 dxe5 11. Be3 Qc7 12. h3 Nf8 13. Rfd1 Ne6 14. c5 Nd7 15. b4 b6 16. cxb6 axb6 17. Rac1 "
    "Bb7 18. Nd5 Qb8 19. Nxb6 Nxb6 20. Bxb6 Nd4 21. Nxd4 exd4 22. Bxd4 Bf8 23. Bc5 Bg7 24. a4 Ba6 "
    "25. a5 Bb5 26. Bf1 Qb7 27. Bxb5 Qxb5 28. Rd7 Qb8 29. Qc4 Rf8 30. Bxf8 Qxf8 31. Qxc6 Rb8 32. "
    "b5 Bb2 33. Rcd1 Bg7 34. b6 h5 35. b7 h4 36. g4 f5 37. Qc7 fxg4 38. Qxb8 Qxb8 39. Rd8+ Qxd8 "
    "40. Rxd8+ Kh7 41. b8=Q Be5 42. Rd7+ Bg7 43. Qe5 Kh6 44. Qxg7+ Kg5 45. Rd5+ Kf4 46. Qe5+ Kf3 "
    "47. Qd4 Ke2 48. Qe3# 1-0",
    // Game 7
    "1. c4 b6 2. d3 Bb7 3. e4 e6 4. Nc3 d5 5. cxd5 exd5 6. exd5 Nf6 7. Be2 Nxd5 8. Bf3 Bb4 9. Ne2 "
    "O-O 10. O-O Re8 11. a3 Bxc3 12. bxc3 c5 13. c4 Rxe2 14. Qxe2 Nc3 15. Qe5 Bxf3 16. gxf3 Na4 "
    "17. Qxc5 Nxc5 0-1",
    // Game 8
    "1. Nf3 f5 2. d4 Nf6 3. Nc3 g6 4. Bf4 Bg7 5. e3 d6 6. h3 O-O 7. Bc4+ Kh8 8. Qe2 Nc6 9. O-O-O "
    "e6 10. Bb3 a6 11. g4 b5 12. gxf5 exf5 13. h4 Nh5 14. Bg5 Qe8 15. Nd5 Ra7 16. Nd2 Nxd4 17. Qd3 "
    "Nxb3+ 18. axb3 a5 19. Nf4 a4 20. bxa4 Rxa4 21. Nb3 Nxf4 22. Bxf4 Be6 23. h5 g5 24. h6 Bf6 25. "
    "Bg3 Bxb3 26. cxb3 Ra2 27. Rde1 Qa8 28. e4 Rxb2 0-1",
    // Game 9
    "1. d4 e5 2. dxe5 Nc6 3. Nf3 Qe7 4. Bf4 Qb4+ 5. Bd2 Qxb2 6. e3 Qxa1 0-1",

};
