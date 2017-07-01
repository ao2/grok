/*
*    Copyright (C) 2016-2017 Grok Image Compression Inc.
*
*    This source code is free software: you can redistribute it and/or  modify
*    it under the terms of the GNU Affero General Public License, version 3,
*    as published by the Free Software Foundation.
*
*    This source code is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*
*    This source code incorporates work covered by the following copyright and
*    permission notice:
*
*    Copyright (C) 2016-2017 Grok Image Compression Inc.
*
*    This source code is free software: you can redistribute it and/or  modify
*    it under the terms of the GNU Affero General Public License, version 3,
*    as published by the Free Software Foundation.
*
*    This source code is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include <vector>

#pragma once
namespace grk {


/*
Smart wrapper to low level C array
*/

struct min_buf_t {
    uint8_t *buf;		/* internal array*/
    uint16_t len;		/* length of array */
};

/*
Copy all segments, in sequence, into contiguous array
*/
bool min_buf_vec_copy_to_contiguous_buffer(grok_vec_t* min_buf_vec, uint8_t* buffer);

/*
Push buffer to back of min buf vector
*/
bool min_buf_vec_push_back(grok_vec_t* buf_vec, uint8_t* buf, uint16_t len);

/*
Sum lengths of all buffers
*/

uint16_t min_buf_vec_get_len(grok_vec_t* min_buf_vec);


/*
Increment buffer offset
*/
void buf_incr_offset(buf_t* buf, uint64_t off);

/*
Free buffer and also its internal array if owns_data is true
*/
void buf_free(buf_t* buf);


/*  Segmented Buffer Interface

A segmented buffer stores a list of buffers, or segments, but can be treated as one single
contiguous buffer.

*/
struct seg_buf_t {
	seg_buf_t();
	~seg_buf_t();
    size_t data_len;	/* total length of all segments*/
    size_t cur_seg_id;	/* current index into segments vector */
	std::vector<buf_t*> segments;
};

/*
Wrap existing array and add to the back of the segmented buffer.
*/
bool seg_buf_push_back(seg_buf_t* seg_buf, uint8_t* buf, size_t len);

/*
Allocate array and add to the back of the segmented buffer
*/
bool seg_buf_alloc_and_push_back(seg_buf_t* seg_buf, size_t len);

/*
Increment offset of current segment
*/
void seg_buf_incr_cur_seg_offset(seg_buf_t* seg_buf, uint64_t offset);

/*
Get length of current segment
*/
size_t seg_buf_get_cur_seg_len(seg_buf_t* seg_buf);

/*
Get offset of current segment
*/
int64_t seg_buf_get_cur_seg_offset(seg_buf_t* seg_buf);

/*
Treat segmented buffer as single contiguous buffer, and get current pointer
*/
uint8_t* seg_buf_get_global_ptr(seg_buf_t* seg_buf);

/*
Treat segmented buffer as single contiguous buffer, and get current offset
*/
int64_t seg_buf_get_global_offset(seg_buf_t* seg_buf);

/*
Reset all offsets to zero, and set current segment to beginning of list
*/
void seg_buf_rewind(seg_buf_t* seg_buf);

/*
Copy all segments, in sequence, into contiguous array
*/
bool seg_buf_copy_to_contiguous_buffer(seg_buf_t* seg_buf, uint8_t* buffer);

/*
Cleans up internal resources
*/
void	seg_buf_cleanup(seg_buf_t* seg_buf);

/*
Return current pointer, stored in ptr variable, and advance segmented buffer
offset by chunk_len

*/
bool seg_buf_zero_copy_read(seg_buf_t* seg_buf,
                                uint8_t** ptr,
                                size_t chunk_len);



}
