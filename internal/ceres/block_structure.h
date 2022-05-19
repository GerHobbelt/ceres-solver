// Ceres Solver - A fast non-linear least squares minimizer
// Copyright 2015 Google Inc. All rights reserved.
// http://ceres-solver.org/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of Google Inc. nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Author: sameeragarwal@google.com (Sameer Agarwal)
//
// Block structure objects are used to carry information about the
// dense block structure of sparse matrices. The BlockSparseMatrix
// object uses the BlockStructure objects to keep track of the matrix
// structure and operate upon it. This allows us to use more cache
// friendly block oriented linear algebra operations on the matrix
// instead of accessing it one scalar entry at a time.

#ifndef CERES_INTERNAL_BLOCK_STRUCTURE_H_
#define CERES_INTERNAL_BLOCK_STRUCTURE_H_

#include <cstdint>
#include <vector>

#include "ceres/internal/export.h"

namespace ceres::internal {

using BlockSize = int32_t;

struct CERES_NO_EXPORT Block {
  Block() = default;
  Block(int size_, int position_) : size(size_), position(position_) {}

  BlockSize size{-1};
  int position{-1};  // Position along the row/column.
};

struct CERES_NO_EXPORT Cell {
  Cell() = default;
  Cell(int block_id_, int position_)
      : block_id(block_id_), position(position_) {}

  // Column or row block id as the case maybe.
  int block_id{-1};
  // Where in the values array of the jacobian is this cell located.
  int position{-1};
};

// Order cell by their block_id;
CERES_NO_EXPORT bool CellLessThan(const Cell& lhs, const Cell& rhs);

struct CERES_NO_EXPORT CompressedList {
  CompressedList() = default;

  // Construct a CompressedList with the cells containing num_cells
  // entries.
  explicit CompressedList(int num_cells) : cells(num_cells) {}
  Block block;
  std::vector<Cell> cells;
};

using CompressedRow = CompressedList;
using CompressedColumn = CompressedList;

struct CERES_NO_EXPORT CompressedRowBlockStructure {
  std::vector<Block> cols;
  std::vector<CompressedRow> rows;
};

struct CERES_NO_EXPORT CompressedColumnBlockStructure {
  std::vector<Block> rows;
  std::vector<CompressedColumn> cols;
};

}  // namespace ceres::internal

#endif  // CERES_INTERNAL_BLOCK_STRUCTURE_H_
