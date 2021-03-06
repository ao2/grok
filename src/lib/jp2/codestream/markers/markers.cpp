/*
 *    Copyright (C) 2016-2020 Grok Image Compression Inc.
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
 * The copyright in this software is being made available under the 2-clauses
 * BSD License, included below. This software may be subject to other third
 * party and contributor rights, including patent rights, and no such rights
 * are granted under this license.
 *
 * Copyright (c) 2002-2014, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2014, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux
 * Copyright (c) 2003-2014, Antonin Descampe
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
 * Copyright (c) 2006-2007, Parvatha Elangovan
 * Copyright (c) 2008, Jerome Fimes, Communications & Systemes <jerome.fimes@c-s.fr>
 * Copyright (c) 2011-2012, Centre National d'Etudes Spatiales (CNES), France
 * Copyright (c) 2012, CS Systemes d'Information, France
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "grok_includes.h"

namespace grk {

/**
 * Copies the tile component parameters of all the component
 * from the first tile component.
 *
 * @param               codeStream           the J2k codec.
 */
static void j2k_copy_tile_component_parameters(CodeStream *codeStream);

static const j2k_mct_function j2k_mct_read_functions_to_float[] = {
		j2k_read_int16_to_float, j2k_read_int32_to_float,
		j2k_read_float32_to_float, j2k_read_float64_to_float };

static const j2k_mct_function j2k_mct_read_functions_to_int32[] = {
		j2k_read_int16_to_int32, j2k_read_int32_to_int32,
		j2k_read_float32_to_int32, j2k_read_float64_to_int32 };

template<typename S, typename D> void j2k_write(const void *p_src_data,
		void *p_dest_data, uint64_t nb_elem) {
	uint8_t *dest_data = (uint8_t*) p_dest_data;
	S *src_data = (S*) p_src_data;
	uint32_t i;
	D temp;
	for (i = 0; i < nb_elem; ++i) {
		temp = (D) *(src_data++);
		grk_write<D>(dest_data, temp, sizeof(D));
		dest_data += sizeof(D);
	}
}

void j2k_read_int16_to_float(const void *p_src_data, void *p_dest_data,
		uint64_t nb_elem) {
	j2k_write<int16_t, float>(p_src_data, p_dest_data, nb_elem);
}
void j2k_read_int32_to_float(const void *p_src_data, void *p_dest_data,
		uint64_t nb_elem) {
	j2k_write<int32_t, float>(p_src_data, p_dest_data, nb_elem);
}
void j2k_read_float32_to_float(const void *p_src_data, void *p_dest_data,
		uint64_t nb_elem) {
	j2k_write<float, float>(p_src_data, p_dest_data, nb_elem);
}
void j2k_read_float64_to_float(const void *p_src_data, void *p_dest_data,
		uint64_t nb_elem) {
	j2k_write<double, float>(p_src_data, p_dest_data, nb_elem);
}
void j2k_read_int16_to_int32(const void *p_src_data, void *p_dest_data,
		uint64_t nb_elem) {
	j2k_write<int16_t, int32_t>(p_src_data, p_dest_data, nb_elem);
}
void j2k_read_int32_to_int32(const void *p_src_data, void *p_dest_data,
		uint64_t nb_elem) {
	j2k_write<int32_t, int32_t>(p_src_data, p_dest_data, nb_elem);
}
void j2k_read_float32_to_int32(const void *p_src_data, void *p_dest_data,
		uint64_t nb_elem) {
	j2k_write<float, int32_t>(p_src_data, p_dest_data, nb_elem);
}
void j2k_read_float64_to_int32(const void *p_src_data, void *p_dest_data,
		uint64_t nb_elem) {
	j2k_write<double, int32_t>(p_src_data, p_dest_data, nb_elem);
}
void j2k_write_float_to_int16(const void *p_src_data, void *p_dest_data,
		uint64_t nb_elem) {
	j2k_write<float, int16_t>(p_src_data, p_dest_data, nb_elem);
}
void j2k_write_float_to_int32(const void *p_src_data, void *p_dest_data,
		uint64_t nb_elem) {
	j2k_write<float, int32_t>(p_src_data, p_dest_data, nb_elem);
}
void j2k_write_float_to_float(const void *p_src_data, void *p_dest_data,
		uint64_t nb_elem) {
	j2k_write<float, float>(p_src_data, p_dest_data, nb_elem);
}
void j2k_write_float_to_float64(const void *p_src_data, void *p_dest_data,
		uint64_t nb_elem) {
	j2k_write<float, double>(p_src_data, p_dest_data, nb_elem);
}

/**************************
 * Read/Write Markers
 *************************/
bool j2k_add_mhmarker(grk_codestream_index *cstr_index, uint32_t type,
		uint64_t pos, uint32_t len) {
	assert(cstr_index != nullptr);

	/* expand the list? */
	if ((cstr_index->marknum + 1) > cstr_index->maxmarknum) {
		grk_marker_info *new_marker;
		cstr_index->maxmarknum = (uint32_t) (100
				+ (float) cstr_index->maxmarknum);
		new_marker = (grk_marker_info*) grk_realloc(cstr_index->marker,
				cstr_index->maxmarknum * sizeof(grk_marker_info));
		if (!new_marker) {
			grok_free(cstr_index->marker);
			cstr_index->marker = nullptr;
			cstr_index->maxmarknum = 0;
			cstr_index->marknum = 0;
			/* GROK_ERROR( "Not enough memory to add mh marker"); */
			return false;
		}
		cstr_index->marker = new_marker;
	}

	/* add the marker */
	cstr_index->marker[cstr_index->marknum].type = (uint16_t) type;
	cstr_index->marker[cstr_index->marknum].pos = (uint64_t) pos;
	cstr_index->marker[cstr_index->marknum].len = (uint32_t) len;
	cstr_index->marknum++;
	return true;
}

bool j2k_write_soc(CodeStream *codeStream, BufferedStream *stream) {
	assert(stream != nullptr);
	assert(codeStream != nullptr);

	(void) codeStream;
	return stream->write_short(J2K_MS_SOC);
}

/**
 * Reads a SOC marker (Start of Codestream)
 * @param       codeStream           the JPEG 2000 file codec.
 * @param       stream        FIXME DOC

 */
bool j2k_read_soc(CodeStream *codeStream, BufferedStream *stream) {
	uint8_t data[2];
	uint32_t marker;

	assert(codeStream != nullptr);
	assert(stream != nullptr);

	if (stream->read(data, 2) != 2)
		return false;

	grk_read<uint32_t>(data, &marker, 2);
	if (marker != J2K_MS_SOC)
		return false;

	/* Next marker should be a SIZ marker in the main header */
	codeStream->m_specific_param.m_decoder.m_state = J2K_DEC_STATE_MH_SIZ;

	if (codeStream->cstr_index) {
		/* FIXME move it in a index structure included in codeStream*/
		codeStream->cstr_index->main_head_start = stream->tell() - 2;

		//event_msg( EVT_INFO, "Start to read j2k main header (%d).", codeStream->cstr_index->main_head_start);

		/* Add the marker to the code stream index*/
		if (!j2k_add_mhmarker(codeStream->cstr_index, J2K_MS_SOC,
				codeStream->cstr_index->main_head_start, 2)) {
			GROK_ERROR("Not enough memory to add mh marker");
			return false;
		}
	}
	return true;
}

bool j2k_write_siz(CodeStream *codeStream, BufferedStream *stream) {
	SIZMarker siz;

	return siz.write(codeStream, stream);
}

/**
 * Reads a CAP marker
 * @param       codeStream           the JPEG 2000 file codec.
 * @param       p_header_data   the data contained in the SIZ box.
 * @param       header_size   the size of the data contained in the SIZ marker.

 */
bool j2k_read_cap(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	CodingParams *cp = &(codeStream->m_cp);

	if (header_size < 6) {
		GROK_ERROR("Error with SIZ marker size");
		return false;
	}

	uint32_t tmp;
	grk_read<uint32_t>(p_header_data, &tmp, 4); /* Pcap */
	bool validPcap = true;
	if (tmp & 0xFFFDFFFF) {
		GROK_WARN("Pcap in CAP marker has unsupported options.");
	}
	if ((tmp & 0x00020000) == 0) {
		GROK_WARN("Pcap in CAP marker should have its 15th MSB set. "
				" Ignoring CAP.");
		validPcap = false;
	}
	if (validPcap) {
		cp->pcap = tmp;
		grk_read<uint32_t>(p_header_data, &tmp, 2); /* Ccap */
		cp->ccap = (uint16_t) tmp;
	}

	return true;
}

bool j2k_write_cap(CodeStream *codeStream, BufferedStream *stream) {
	assert(codeStream != nullptr);
	assert(stream != nullptr);

	auto cp = &(codeStream->m_cp);
	auto tcp = &cp->tcps[0];
	auto tccp0 = &tcp->tccps[0];

	//marker size excluding header
	uint16_t Lcap = 8;

	uint32_t Pcap = 0x00020000; //for jph, Pcap^15 must be set, the 15th MSB
	uint16_t Ccap[32]; //a maximum of 32
	memset(Ccap, 0, sizeof(Ccap));

	bool reversible = tccp0->qmfbid == 1;
	if (reversible)
		Ccap[0] &= 0xFFDF;
	else
		Ccap[0] |= 0x0020;
	Ccap[0] &= 0xFFE0;

	uint32_t Bp = 0;
	uint32_t B = tcp->qcd.get_MAGBp();
	if (B <= 8)
		Bp = 0;
	else if (B < 28)
		Bp = B - 8;
	else if (B < 48)
		Bp = 13 + (B >> 2);
	else
		Bp = 31;
	Ccap[0] = (uint16_t) (Ccap[0] | Bp);

	/* CAP */
	if (!stream->write_short(J2K_MS_CAP)) {
		return false;
	}

	/* L_CAP */
	if (!stream->write_short(Lcap))
		return false;
	/* PCAP */
	if (!stream->write_int(Pcap))
		return false;
	/* CCAP */
	if (!stream->write_short(Ccap[0]))
		return false;

	return true;
}

/**
 * Reads a SIZ marker (image and tile size)
 * @param       codeStream           the JPEG 2000 file codec.
 * @param       p_header_data   the data contained in the SIZ box.
 * @param       header_size   the size of the data contained in the SIZ marker.

 */
bool j2k_read_siz(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	SIZMarker siz;

	return siz.read(codeStream, p_header_data, header_size);
}

bool j2k_write_com(CodeStream *codeStream, BufferedStream *stream) {
	assert(codeStream != nullptr);
	assert(stream != nullptr);

	for (uint32_t i = 0; i < codeStream->m_cp.num_comments; ++i) {
		const char *comment = codeStream->m_cp.comment[i];
		uint16_t comment_size = codeStream->m_cp.comment_len[i];
		if (!comment_size) {
			GROK_WARN("Empty comment. Ignoring");
			continue;
		}
		if (comment_size > GRK_MAX_COMMENT_LENGTH) {
			GROK_WARN(
					"Comment length %s is greater than maximum comment length %d. Ignoring",
					comment_size, GRK_MAX_COMMENT_LENGTH);
			continue;
		}
		uint32_t totacom_size = (uint32_t) comment_size + 6;

		/* COM */
		if (!stream->write_short(J2K_MS_COM))
			return false;
		/* L_COM */
		if (!stream->write_short((uint16_t) (totacom_size - 2)))
			return false;
		if (!stream->write_short(codeStream->m_cp.isBinaryComment[i] ? 0 : 1))
			return false;
		if (!stream->write_bytes((uint8_t*) comment, comment_size))
			return false;
	}

	return true;
}

/**
 * Reads a COM marker (comments)
 * @param       codeStream           the JPEG 2000 file codec.
 * @param       p_header_data   the data contained in the COM box.
 * @param       header_size   the size of the data contained in the COM marker.

 */
bool j2k_read_com(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	assert(codeStream != nullptr);
	assert(p_header_data != nullptr);
	assert(header_size != 0);

	if (header_size < 2) {
		GROK_ERROR("j2k_read_com: Corrupt COM segment ");
		return false;
	} else if (header_size == 2) {
		GROK_WARN("j2k_read_com: Empty COM segment. Ignoring ");
		return true;
	}
	if (codeStream->m_cp.num_comments == GRK_NUM_COMMENTS_SUPPORTED) {
		GROK_WARN("j2k_read_com: Only %d comments are supported. Ignoring",
		GRK_NUM_COMMENTS_SUPPORTED);
		return true;
	}

	uint32_t commentType;
	grk_read<uint32_t>(p_header_data, &commentType, 2);
	auto numComments = codeStream->m_cp.num_comments;
	codeStream->m_cp.isBinaryComment[numComments] = (commentType == 0);
	if (commentType > 1) {
		GROK_WARN(
				"j2k_read_com: Unrecognized comment type 0x%x. Assuming IS 8859-15:1999 (Latin) values",
				commentType);
	}

	p_header_data += 2;
	uint16_t commentSize = (uint16_t) (header_size - 2);
	size_t commentSizeToAlloc = commentSize;
	if (!codeStream->m_cp.isBinaryComment[numComments])
		commentSizeToAlloc++;
	codeStream->m_cp.comment[numComments] = (char*) grk_buffer_new(
			commentSizeToAlloc);
	if (!codeStream->m_cp.comment[numComments]) {
		GROK_ERROR(
				"j2k_read_com: Out of memory when allocating memory for comment ");
		return false;
	}
	memcpy(codeStream->m_cp.comment[numComments], p_header_data, commentSize);
	codeStream->m_cp.comment_len[numComments] = commentSize;

	// make null-terminated string
	if (!codeStream->m_cp.isBinaryComment[numComments])
		codeStream->m_cp.comment[numComments][commentSize] = 0;
	codeStream->m_cp.num_comments++;
	return true;
}

bool j2k_write_cod(CodeStream *codeStream, BufferedStream *stream) {
	uint32_t code_size;

	assert(codeStream != nullptr);
	assert(stream != nullptr);

	auto cp = &(codeStream->m_cp);
	auto tcp = &cp->tcps[codeStream->m_tileProcessor->m_current_tile_index];
	code_size = 9
			+ j2k_get_SPCod_SPCoc_size(codeStream,
					codeStream->m_tileProcessor->m_current_tile_index, 0);

	/* COD */
	if (!stream->write_short(J2K_MS_COD))
		return false;
	/* L_COD */
	if (!stream->write_short((uint16_t) (code_size - 2)))
		return false;
	/* Scod */
	if (!stream->write_byte((uint8_t) tcp->csty))
		return false;
	/* SGcod (A) */
	if (!stream->write_byte((uint8_t) tcp->prg))
		return false;
	/* SGcod (B) */
	if (!stream->write_short((uint16_t) tcp->numlayers))
		return false;
	/* SGcod (C) */
	if (!stream->write_byte((uint8_t) tcp->mct))
		return false;
	if (!j2k_write_SPCod_SPCoc(codeStream,
			codeStream->m_tileProcessor->m_current_tile_index, 0, stream)) {
		GROK_ERROR("Error writing COD marker");
		return false;
	}

	return true;
}

/**
 * Reads a COD marker (Coding Style defaults)
 * @param       p_header_data   the data contained in the COD box.
 * @param       codeStream                   JPEG 2000 code stream
 * @param       header_size   the size of the data contained in the COD marker.

 */
bool j2k_read_cod(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	/* loop */
	uint32_t i;
	uint32_t tmp;
	assert(p_header_data != nullptr);
	assert(codeStream != nullptr);

	auto image = codeStream->m_private_image;
	auto cp = &(codeStream->m_cp);

	/* If we are in the first tile-part header of the current tile */
	auto tcp = codeStream->get_current_decode_tcp();

	/* Only one COD per tile */
	if (tcp->cod) {
		GROK_WARN(
				"Multiple COD markers detected for tile part %d. The JPEG 2000 standard does not allow more than one COD marker per tile.",
				tcp->m_current_tile_part_index);
	}
	tcp->cod = true;

	/* Make sure room is sufficient */
	if (header_size < cod_coc_len) {
		GROK_ERROR("Error reading COD marker");
		return false;
	}

	grk_read<uint32_t>(p_header_data, &tcp->csty, 1); /* Scod */
	++p_header_data;
	/* Make sure we know how to decompress this */
	if ((tcp->csty
			& ~(uint32_t) (J2K_CP_CSTY_PRT | J2K_CP_CSTY_SOP | J2K_CP_CSTY_EPH))
			!= 0U) {
		GROK_ERROR("Unknown Scod value in COD marker");
		return false;
	}
	grk_read<uint32_t>(p_header_data, &tmp, 1); /* SGcod (A) */
	++p_header_data;
	tcp->prg = (GRK_PROG_ORDER) tmp;
	/* Make sure progression order is valid */
	if (tcp->prg > GRK_CPRL) {
		GROK_ERROR("Unknown progression order in COD marker");
		tcp->prg = GRK_PROG_UNKNOWN;
	}
	grk_read<uint32_t>(p_header_data, &tcp->numlayers, 2); /* SGcod (B) */
	p_header_data += 2;

	if ((tcp->numlayers < 1U) || (tcp->numlayers > USHRT_MAX)) {
		GROK_ERROR(
				"Invalid number of layers in COD marker : %d not in range [1-65535]",
				tcp->numlayers);
		return false;
	}

	/* If user didn't set a number layer to decompress take the max specify in the code stream. */
	if (cp->m_coding_params.m_dec.m_layer) {
		tcp->num_layers_to_decode = cp->m_coding_params.m_dec.m_layer;
	} else {
		tcp->num_layers_to_decode = tcp->numlayers;
	}

	grk_read<uint32_t>(p_header_data, &tcp->mct, 1); /* SGcod (C) */
	++p_header_data;
	if (tcp->mct > 1) {
		GROK_ERROR("Invalid MCT value : %d. Should be either 0 or 1", tcp->mct);
		return false;
	}
	header_size = (uint16_t) (header_size - cod_coc_len);
	for (i = 0; i < image->numcomps; ++i) {
		tcp->tccps[i].csty = tcp->csty & J2K_CCP_CSTY_PRT;
	}

	if (!j2k_read_SPCod_SPCoc(codeStream, 0, p_header_data, &header_size)) {
		GROK_ERROR("Error reading COD marker");
		return false;
	}

	if (header_size != 0) {
		GROK_ERROR("Error reading COD marker");
		return false;
	}

	/* Apply the coding style to other components of the current tile or the m_default_tcp*/
	j2k_copy_tile_component_parameters(codeStream);

	return true;
}

static void j2k_copy_tile_component_parameters(CodeStream *codeStream) {
	/* loop */
	uint32_t i;
	uint32_t prc_size;

	assert(codeStream != nullptr);

	auto tcp = codeStream->get_current_decode_tcp();
	auto ref_tccp = &tcp->tccps[0];
	auto copied_tccp = ref_tccp + 1;
	prc_size = ref_tccp->numresolutions * (uint32_t) sizeof(uint32_t);

	for (i = 1; i < codeStream->m_private_image->numcomps; ++i) {
		copied_tccp->numresolutions = ref_tccp->numresolutions;
		copied_tccp->cblkw = ref_tccp->cblkw;
		copied_tccp->cblkh = ref_tccp->cblkh;
		copied_tccp->cblk_sty = ref_tccp->cblk_sty;
		copied_tccp->qmfbid = ref_tccp->qmfbid;
		memcpy(copied_tccp->prcw, ref_tccp->prcw, prc_size);
		memcpy(copied_tccp->prch, ref_tccp->prch, prc_size);
		++copied_tccp;
	}
}

bool j2k_write_coc(CodeStream *codeStream, uint32_t comp_no,
		BufferedStream *stream) {
	assert(codeStream != nullptr);
	assert(stream != nullptr);
	uint32_t coc_size;
	uint32_t comp_room;

	assert(codeStream != nullptr);

	auto cp = &(codeStream->m_cp);
	auto tcp = &cp->tcps[codeStream->m_tileProcessor->m_current_tile_index];
	auto image = codeStream->m_private_image;
	comp_room = (image->numcomps <= 256) ? 1 : 2;
	coc_size = cod_coc_len + comp_room
			+ j2k_get_SPCod_SPCoc_size(codeStream,
					codeStream->m_tileProcessor->m_current_tile_index, comp_no);

	/* COC */
	if (!stream->write_short(J2K_MS_COC))
		return false;
	/* L_COC */
	if (!stream->write_short((uint16_t) (coc_size - 2)))
		return false;
	/* Ccoc */
	if (comp_room == 2) {
		if (!stream->write_short((uint16_t) comp_no))
			return false;
	} else {
		if (!stream->write_byte((uint8_t) comp_no))
			return false;
	}

	/* Scoc */
	if (!stream->write_byte((uint8_t) tcp->tccps[comp_no].csty))
		return false;

	return j2k_write_SPCod_SPCoc(codeStream,
			codeStream->m_tileProcessor->m_current_tile_index, 0, stream);

}

bool j2k_compare_coc(CodeStream *codeStream, uint32_t first_comp_no,
		uint32_t second_comp_no) {
	CodingParams *cp = nullptr;
	TileCodingParams *tcp = nullptr;

	assert(codeStream != nullptr);

	cp = &(codeStream->m_cp);
	tcp = &cp->tcps[codeStream->m_tileProcessor->m_current_tile_index];

	if (tcp->tccps[first_comp_no].csty != tcp->tccps[second_comp_no].csty) {
		return false;
	}
	return j2k_compare_SPCod_SPCoc(codeStream,
			codeStream->m_tileProcessor->m_current_tile_index, first_comp_no,
			second_comp_no);
}

/**
 * Reads a COC marker (Coding Style Component)
 * @param       codeStream                   JPEG 2000 code stream
 * @param       p_header_data   the data contained in the COC box.
 * @param       header_size   the size of the data contained in the COC marker.

 */
bool j2k_read_coc(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	uint32_t comp_room;
	uint32_t comp_no;

	assert(p_header_data != nullptr);
	assert(codeStream != nullptr);

	auto tcp = codeStream->get_current_decode_tcp();
	auto image = codeStream->m_private_image;

	comp_room = image->numcomps <= 256 ? 1 : 2;

	/* make sure room is sufficient*/
	if (header_size < comp_room + 1) {
		GROK_ERROR("Error reading COC marker");
		return false;
	}
	header_size = (uint16_t) (header_size - (comp_room + 1));

	grk_read<uint32_t>(p_header_data, &comp_no, comp_room); /* Ccoc */
	p_header_data += comp_room;
	if (comp_no >= image->numcomps) {
		GROK_ERROR("Error reading COC marker (bad number of components)");
		return false;
	}

	tcp->tccps[comp_no].csty = *p_header_data++; /* Scoc */

	if (!j2k_read_SPCod_SPCoc(codeStream, comp_no, p_header_data, &header_size)) {
		GROK_ERROR("Error reading COC marker");
		return false;
	}

	if (header_size != 0) {
		GROK_ERROR("Error reading COC marker");
		return false;
	}
	return true;
}

bool j2k_write_qcd(CodeStream *codeStream, BufferedStream *stream) {
	uint32_t qcd_size;

	assert(codeStream != nullptr);
	assert(stream != nullptr);

	qcd_size = 4
			+ j2k_get_SQcd_SQcc_size(codeStream,
					codeStream->m_tileProcessor->m_current_tile_index, 0);

	/* QCD */
	if (!stream->write_short(J2K_MS_QCD))
		return false;
	/* L_QCD */
	if (!stream->write_short((uint16_t) (qcd_size - 2)))
		return false;
	if (!j2k_write_SQcd_SQcc(codeStream,
			codeStream->m_tileProcessor->m_current_tile_index, 0, stream)) {
		GROK_ERROR("Error writing QCD marker");
		return false;
	}

	return true;
}

/**
 * Reads a QCD marker (Quantization defaults)
 * @param       codeStream           JPEG 2000 code stream
 * @param       p_header_data   the data contained in the QCD box.
 * @param       header_size   the size of the data contained in the QCD marker.

 */
bool j2k_read_qcd(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	assert(p_header_data != nullptr);
	assert(codeStream != nullptr);

	if (!j2k_read_SQcd_SQcc(false, codeStream, 0, p_header_data, &header_size)) {
		GROK_ERROR("Error reading QCD marker");
		return false;
	}
	if (header_size != 0) {
		GROK_ERROR("Error reading QCD marker");
		return false;
	}

	// Apply the quantization parameters to the other components
	// of the current tile or m_default_tcp
	auto tcp = codeStream->get_current_decode_tcp();
	auto ref_tccp = tcp->tccps;
	for (uint32_t i = 1; i < codeStream->m_private_image->numcomps; ++i) {
		auto target_tccp = ref_tccp + i;
		target_tccp->quant.apply_quant(ref_tccp, target_tccp);
	}
	return true;
}

bool j2k_write_qcc(CodeStream *codeStream, uint32_t comp_no,
		BufferedStream *stream) {
	assert(codeStream != nullptr);
	assert(stream != nullptr);
	uint32_t qcc_size = 6
			+ j2k_get_SQcd_SQcc_size(codeStream,
					codeStream->m_tileProcessor->m_current_tile_index, comp_no);

	/* QCC */
	if (!stream->write_short(J2K_MS_QCC)) {
		return false;
	}

	if (codeStream->m_private_image->numcomps <= 256) {
		--qcc_size;

		/* L_QCC */
		if (!stream->write_short((uint16_t) (qcc_size - 2)))
			return false;
		/* Cqcc */
		if (!stream->write_byte((uint8_t) comp_no))
			return false;
	} else {
		/* L_QCC */
		if (!stream->write_short((uint16_t) (qcc_size - 2)))
			return false;
		/* Cqcc */
		if (!stream->write_short((uint16_t) comp_no))
			return false;
	}

	return j2k_write_SQcd_SQcc(codeStream,
			codeStream->m_tileProcessor->m_current_tile_index, comp_no, stream);
}

bool j2k_compare_qcc(CodeStream *codeStream, uint32_t first_comp_no,
		uint32_t second_comp_no) {
	return j2k_compare_SQcd_SQcc(codeStream,
			codeStream->m_tileProcessor->m_current_tile_index, first_comp_no,
			second_comp_no);
}

/**
 * Reads a QCC marker (Quantization component)
 * @param       codeStream           JPEG 2000 code stream
 * @param       p_header_data   the data contained in the QCC box.
 * @param       header_size   the size of the data contained in the QCC marker.

 */
bool j2k_read_qcc(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	uint32_t num_comp, comp_no;

	assert(p_header_data != nullptr);
	assert(codeStream != nullptr);

	num_comp = codeStream->m_private_image->numcomps;

	if (num_comp <= 256) {
		if (header_size < 1) {
			GROK_ERROR("Error reading QCC marker");
			return false;
		}
		grk_read<uint32_t>(p_header_data, &comp_no, 1);
		++p_header_data;
		--header_size;
	} else {
		if (header_size < 2) {
			GROK_ERROR("Error reading QCC marker");
			return false;
		}
		grk_read<uint32_t>(p_header_data, &comp_no, 2);
		p_header_data += 2;
		header_size = (uint16_t) (header_size - 2);
	}

	if (comp_no >= codeStream->m_private_image->numcomps) {
		GROK_ERROR(
				"Invalid component number: %d, regarding the number of components %d",
				comp_no, codeStream->m_private_image->numcomps);
		return false;
	}

	if (!j2k_read_SQcd_SQcc(true, codeStream, comp_no, p_header_data,
			&header_size)) {
		GROK_ERROR("Error reading QCC marker");
		return false;
	}

	if (header_size != 0) {
		GROK_ERROR("Error reading QCC marker");
		return false;
	}

	return true;
}

uint16_t getPocSize(uint32_t nb_comp, uint32_t nb_poc) {
	uint32_t poc_room = (nb_comp <= 256) ? 1 : 2;

	return (uint16_t) (4 + (5 + 2 * poc_room) * nb_poc);
}

bool j2k_write_poc(CodeStream *codeStream, BufferedStream *stream) {
	assert(codeStream != nullptr);

	auto tcp = &codeStream->m_cp.tcps[codeStream->m_tileProcessor->m_current_tile_index];
	auto tccp = &tcp->tccps[0];
	auto image = codeStream->m_private_image;
	uint32_t nb_comp = image->numcomps;
	uint32_t nb_poc = tcp->numpocs + 1;
	uint32_t poc_room = (nb_comp <= 256) ? 1 : 2;

	auto poc_size = getPocSize(nb_comp, 1 + tcp->numpocs);

	/* POC  */
	if (!stream->write_short(J2K_MS_POC))
		return false;

	/* Lpoc */
	if (!stream->write_short((uint16_t) (poc_size - 2)))
		return false;

	auto current_poc = tcp->pocs;
	for (uint32_t i = 0; i < nb_poc; ++i) {
		/* RSpoc_i */
		if (!stream->write_byte((uint8_t) current_poc->resno0))
			return false;
		/* CSpoc_i */
		if (!stream->write_byte((uint8_t) current_poc->compno0))
			return false;
		/* LYEpoc_i */
		if (!stream->write_short((uint16_t) current_poc->layno1))
			return false;
		/* REpoc_i */
		if (!stream->write_byte((uint8_t) current_poc->resno1))
			return false;
		/* CEpoc_i */
		if (poc_room == 2) {
			if (!stream->write_short((uint16_t) current_poc->compno1))
				return false;
		} else {
			if (!stream->write_byte((uint8_t) current_poc->compno1))
				return false;
		}
		/* Ppoc_i */
		if (!stream->write_byte((uint8_t) current_poc->prg))
			return false;

		/* change the value of the max layer according to the actual number of layers in the file, components and resolutions*/
		current_poc->layno1 = std::min<uint32_t>(current_poc->layno1,
				tcp->numlayers);
		current_poc->resno1 = std::min<uint32_t>(current_poc->resno1,
				tccp->numresolutions);
		current_poc->compno1 = std::min<uint32_t>(current_poc->compno1,
				nb_comp);

		++current_poc;
	}

	return true;
}

/**
 * Reads a POC marker (Progression Order Change)
 *
 * @param       p_header_data   the data contained in the POC box.
 * @param       codeStream                   JPEG 2000 code stream
 * @param       header_size   the size of the data contained in the POC marker.

 */
bool j2k_read_poc(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	uint32_t i, nb_comp, tmp;
	uint32_t old_poc_nb, current_poc_nb, current_poc_remaining;
	uint32_t chunk_size, comp_room;

	assert(p_header_data != nullptr);
	assert(codeStream != nullptr);

	auto image = codeStream->m_private_image;
	nb_comp = image->numcomps;
	comp_room = (nb_comp <= 256) ? 1 : 2;
	chunk_size = 5 + 2 * comp_room;
	current_poc_nb = header_size / chunk_size;
	current_poc_remaining = header_size % chunk_size;

	if ((current_poc_nb <= 0) || (current_poc_remaining != 0)) {
		GROK_ERROR("Error reading POC marker");
		return false;
	}

	auto tcp = codeStream->get_current_decode_tcp();
	old_poc_nb = tcp->POC ? tcp->numpocs + 1 : 0;
	current_poc_nb += old_poc_nb;

	if (current_poc_nb >= 32) {
		GROK_ERROR("Too many POCs %d", current_poc_nb);
		return false;
	}
	assert(current_poc_nb < 32);

	/* now poc is in use.*/
	tcp->POC = true;

	auto current_poc = &tcp->pocs[old_poc_nb];
	for (i = old_poc_nb; i < current_poc_nb; ++i) {
		/* RSpoc_i */
		grk_read<uint32_t>(p_header_data, &(current_poc->resno0), 1);
		++p_header_data;
		/* CSpoc_i */
		grk_read<uint32_t>(p_header_data, &(current_poc->compno0), comp_room);
		p_header_data += comp_room;
		/* LYEpoc_i */
		grk_read<uint32_t>(p_header_data, &(current_poc->layno1), 2);
		/* make sure layer end is in acceptable bounds */
		current_poc->layno1 = std::min<uint32_t>(current_poc->layno1,
				tcp->numlayers);
		p_header_data += 2;
		/* REpoc_i */
		grk_read<uint32_t>(p_header_data, &(current_poc->resno1), 1);
		++p_header_data;
		/* CEpoc_i */
		grk_read<uint32_t>(p_header_data, &(current_poc->compno1), comp_room);
		p_header_data += comp_room;
		/* Ppoc_i */
		grk_read<uint32_t>(p_header_data, &tmp, 1);
		++p_header_data;
		current_poc->prg = (GRK_PROG_ORDER) tmp;
		/* make sure comp is in acceptable bounds */
		current_poc->compno1 = std::min<uint32_t>(current_poc->compno1,
				nb_comp);
		++current_poc;
	}
	tcp->numpocs = current_poc_nb - 1;
	return true;
}

/**
 * Reads a CRG marker (Component registration)
 *
 * @param       codeStream                   JPEG 2000 code stream
 * @param       p_header_data   the data contained in the TLM box.
 * @param       header_size   the size of the data contained in the TLM marker.

 */
bool j2k_read_crg(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	assert(p_header_data != nullptr);
	assert(codeStream != nullptr);

	uint32_t nb_comp = codeStream->m_private_image->numcomps;

	if (header_size != nb_comp * 4) {
		GROK_ERROR("Error reading CRG marker");
		return false;
	}
	uint32_t Xcrg_i, Ycrg_i;
	for (uint32_t i = 0; i < nb_comp; ++i) {
		// Xcrg_i
		grk_read<uint32_t>(p_header_data, &Xcrg_i, 2);
		p_header_data += 2;
		// Xcrg_i
		grk_read<uint32_t>(p_header_data, &Ycrg_i, 2);
		p_header_data += 2;
	}
	return true;
}

/**
 * Reads a PLM marker (Packet length, main header marker)
 *
 * @param       codeStream                   JPEG 2000 code stream
 * @param       p_header_data   the data contained in the TLM box.
 * @param       header_size   the size of the data contained in the TLM marker.

 */
bool j2k_read_plm(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t hdr_size) {
	assert(p_header_data != nullptr);
	assert(codeStream != nullptr);

	if (!codeStream->m_cp.plm_markers)
		codeStream->m_cp.plm_markers = new PacketLengthMarkers();

	return codeStream->m_cp.plm_markers->readPLM(p_header_data, hdr_size);
}

/**
 * Reads a PLT marker (Packet length, tile-part header)
 *
 * @param       codeStream           JPEG 2000 code stream
 * @param       p_header_data   the data contained in the PLT box.
 * @param       header_size   the size of the data contained in the PLT marker.

 */
bool j2k_read_plt(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	assert(p_header_data != nullptr);
	assert(codeStream != nullptr);
	if (!codeStream->m_tileProcessor->plt_markers)
		codeStream->m_tileProcessor->plt_markers = new PacketLengthMarkers();

	return codeStream->m_tileProcessor->plt_markers->readPLT(p_header_data,
			header_size);
}

/**
 * Reads a PPM marker (Packed packet headers, main header)
 *
 * @param       codeStream                   JPEG 2000 code stream
 * @param       p_header_data   the data contained in the POC box.
 * @param       header_size   the size of the data contained in the POC marker.

 */

bool j2k_read_ppm(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	PPMMarker ppm;

	return ppm.read(codeStream, p_header_data, header_size);
}

/**
 * Merges all PPM markers read (Packed headers, main header)
 *
 * @param       p_cp      main coding parameters.

 */
bool j2k_merge_ppm(CodingParams *p_cp) {
	PPMMarker ppm;

	return ppm.merge(p_cp);
}

/**
 * Reads a PPT marker (Packed packet headers, tile-part header)
 *
 * @param       codeStream                   JPEG 2000 code stream
 * @param       p_header_data   the data contained in the PPT box.
 * @param       header_size   the size of the data contained in the PPT marker.

 */
bool j2k_read_ppt(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	uint32_t Z_ppt;

	assert(p_header_data != nullptr);
	assert(codeStream != nullptr);

	/* We need to have the Z_ppt element + 1 byte of Ippt at minimum */
	if (header_size < 2) {
		GROK_ERROR("Error reading PPT marker");
		return false;
	}

	auto cp = &(codeStream->m_cp);
	if (cp->ppm) {
		GROK_ERROR(
				"Error reading PPT marker: packet header have been previously found in the main header (PPM marker).");
		return false;
	}

	auto tcp = &(cp->tcps[codeStream->m_tileProcessor->m_current_tile_index]);
	tcp->ppt = true;

	/* Z_ppt */
	grk_read<uint32_t>(p_header_data, &Z_ppt, 1);
	++p_header_data;
	--header_size;

	/* check allocation needed */
	if (tcp->ppt_markers == nullptr) { /* first PPT marker */
		uint32_t newCount = Z_ppt + 1U; /* can't overflow, Z_ppt is UINT8 */
		assert(tcp->ppt_markers_count == 0U);

		tcp->ppt_markers = (grk_ppx*) grk_calloc(newCount, sizeof(grk_ppx));
		if (tcp->ppt_markers == nullptr) {
			GROK_ERROR("Not enough memory to read PPT marker");
			return false;
		}
		tcp->ppt_markers_count = newCount;
	} else if (tcp->ppt_markers_count <= Z_ppt) {
		uint32_t newCount = Z_ppt + 1U; /* can't overflow, Z_ppt is UINT8 */
		grk_ppx *new_ppt_markers;
		new_ppt_markers = (grk_ppx*) grk_realloc(tcp->ppt_markers,
				newCount * sizeof(grk_ppx));
		if (new_ppt_markers == nullptr) {
			/* clean up to be done on tcp destruction */
			GROK_ERROR("Not enough memory to read PPT marker");
			return false;
		}
		tcp->ppt_markers = new_ppt_markers;
		memset(tcp->ppt_markers + tcp->ppt_markers_count, 0,
				(newCount - tcp->ppt_markers_count) * sizeof(grk_ppx));
		tcp->ppt_markers_count = newCount;
	}

	if (tcp->ppt_markers[Z_ppt].m_data != nullptr) {
		/* clean up to be done on tcp destruction */
		GROK_ERROR("Zppt %u already read", Z_ppt);
		return false;
	}

	tcp->ppt_markers[Z_ppt].m_data = (uint8_t*) grk_malloc(header_size);
	if (tcp->ppt_markers[Z_ppt].m_data == nullptr) {
		/* clean up to be done on tcp destruction */
		GROK_ERROR("Not enough memory to read PPT marker");
		return false;
	}
	tcp->ppt_markers[Z_ppt].m_data_size = header_size;
	memcpy(tcp->ppt_markers[Z_ppt].m_data, p_header_data, header_size);
	return true;
}

/**
 * Merges all PPT markers read (Packed packet headers, tile-part header)
 *
 * @param       p_tcp   the tile.

 */
bool j2k_merge_ppt(TileCodingParams *p_tcp) {
	assert(p_tcp != nullptr);
	assert(p_tcp->ppt_buffer == nullptr);

	if (!p_tcp->ppt)
		return true;

	if (p_tcp->ppt_buffer != nullptr) {
		GROK_ERROR("multiple calls to j2k_merge_ppt()");
		return false;
	}

	uint32_t ppt_data_size = 0U;
	for (uint32_t i = 0U; i < p_tcp->ppt_markers_count; ++i) {
		ppt_data_size += p_tcp->ppt_markers[i].m_data_size; /* can't overflow, max 256 markers of max 65536 bytes */
	}

	p_tcp->ppt_buffer = (uint8_t*) grk_malloc(ppt_data_size);
	if (p_tcp->ppt_buffer == nullptr) {
		GROK_ERROR("Not enough memory to read PPT marker");
		return false;
	}
	p_tcp->ppt_len = ppt_data_size;
	ppt_data_size = 0U;
	for (uint32_t i = 0U; i < p_tcp->ppt_markers_count; ++i) {
		if (p_tcp->ppt_markers[i].m_data != nullptr) { /* standard doesn't seem to require contiguous Zppt */
			memcpy(p_tcp->ppt_buffer + ppt_data_size,
					p_tcp->ppt_markers[i].m_data,
					p_tcp->ppt_markers[i].m_data_size);
			ppt_data_size += p_tcp->ppt_markers[i].m_data_size; /* can't overflow, max 256 markers of max 65536 bytes */

			grok_free(p_tcp->ppt_markers[i].m_data);
			p_tcp->ppt_markers[i].m_data = nullptr;
			p_tcp->ppt_markers[i].m_data_size = 0U;
		}
	}

	p_tcp->ppt_markers_count = 0U;
	grok_free(p_tcp->ppt_markers);
	p_tcp->ppt_markers = nullptr;

	p_tcp->ppt_data = p_tcp->ppt_buffer;
	p_tcp->ppt_data_size = p_tcp->ppt_len;
	return true;
}

bool j2k_read_sot(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	SOTMarker sot;
	return sot.read(codeStream, p_header_data, header_size);
}

bool j2k_read_sod(CodeStream *codeStream, BufferedStream *stream) {
	assert(codeStream != nullptr);
	assert(stream != nullptr);
	auto tileProcessor = codeStream->m_tileProcessor;

	// note: we subtract 2 to account for SOD marker
	TileCodingParams *tcp = codeStream->get_current_decode_tcp();
	if (codeStream->m_specific_param.m_decoder.m_last_tile_part) {
		tileProcessor->tile_part_data_length =
				(uint32_t) (stream->get_number_byte_left() - 2);
	} else {
		if (tileProcessor->tile_part_data_length >= 2)
			tileProcessor->tile_part_data_length -= 2;
	}
	if (tileProcessor->tile_part_data_length) {
		auto bytesLeftInStream = stream->get_number_byte_left();
		// check that there are enough bytes in stream to fill tile data
		if (codeStream->m_tileProcessor->tile_part_data_length > bytesLeftInStream) {
			GROK_WARN("Tile part length %lld greater than "
					"stream length %lld\n"
					"(tile: %d, tile part: %d). Tile may be truncated.",
					tileProcessor->tile_part_data_length,
					stream->get_number_byte_left(),
					tileProcessor->m_current_tile_index,
					tileProcessor->m_current_tile_part_index);

			// sanitize tile_part_data_length
			tileProcessor->tile_part_data_length =	(uint32_t) bytesLeftInStream;
		}
	}
	/* Index */
	grk_codestream_index *cstr_index = codeStream->cstr_index;
	if (cstr_index) {
		uint64_t current_pos = stream->tell();
		if (current_pos < 2) {
			GROK_ERROR("Stream too short");
			return false;
		}
		current_pos = (uint64_t) (current_pos - 2);

		uint32_t current_tile_part =
				cstr_index->tile_index[tileProcessor->m_current_tile_index].current_tpsno;
		cstr_index->tile_index[tileProcessor->m_current_tile_index].tp_index[current_tile_part].end_header =
				current_pos;
		cstr_index->tile_index[tileProcessor->m_current_tile_index].tp_index[current_tile_part].end_pos =
				current_pos + tileProcessor->tile_part_data_length + 2;

		if (!TileLengthMarkers::add_to_index(
				tileProcessor->m_current_tile_index, cstr_index,
				J2K_MS_SOD, current_pos, 0)) {
			GROK_ERROR("Not enough memory to add tl marker");
			return false;
		}

		/*cstr_index->packno = 0;*/
	}
	size_t current_read_size = 0;
	if (tileProcessor->tile_part_data_length) {
		if (!tcp->m_tile_data)
			tcp->m_tile_data = new ChunkBuffer();

		auto len = tileProcessor->tile_part_data_length;
		uint8_t *buff = nullptr;
		auto zeroCopy = stream->supportsZeroCopy();
		if (!zeroCopy) {
			try {
				buff = new uint8_t[len];
			} catch (std::bad_alloc &ex) {
				GROK_ERROR("Not enough memory to allocate segment");
				return false;
			}
		} else {
			buff = stream->getCurrentPtr();
		}
		current_read_size = stream->read(zeroCopy ? nullptr : buff, len);
		tcp->m_tile_data->add_chunk(buff, len, !zeroCopy);

	}
	if (current_read_size != tileProcessor->tile_part_data_length)
		codeStream->m_specific_param.m_decoder.m_state = J2K_DEC_STATE_NO_EOC;
	else
		codeStream->m_specific_param.m_decoder.m_state = J2K_DEC_STATE_TPH_SOT;

	return true;
}

bool j2k_write_rgn(CodeStream *codeStream, uint16_t tile_no, uint32_t comp_no,
		uint32_t nb_comps, BufferedStream *stream) {
	uint32_t rgn_size;

	assert(codeStream != nullptr);
	assert(stream != nullptr);

	auto cp = &(codeStream->m_cp);
	auto tcp = &cp->tcps[tile_no];
	auto tccp = &tcp->tccps[comp_no];
	uint32_t comp_room = (nb_comps <= 256) ? 1 : 2;
	rgn_size = 6 + comp_room;

	/* RGN  */
	if (!stream->write_short(J2K_MS_RGN))
		return false;
	/* Lrgn */
	if (!stream->write_short((uint16_t) (rgn_size - 2)))
		return false;
	/* Crgn */
	if (comp_room == 2) {
		if (!stream->write_short((uint16_t) comp_no))
			return false;
	} else {
		if (!stream->write_byte((uint8_t) comp_no))
			return false;
	}
	/* Srgn */
	if (!stream->write_byte(0))
		return false;

	/* SPrgn */
	return stream->write_byte((uint8_t) tccp->roishift);
}

bool j2k_write_eoc(CodeStream *codeStream, BufferedStream *stream) {
	assert(codeStream != nullptr);
	assert(stream != nullptr);
	(void) codeStream;
	if (!stream->write_short(J2K_MS_EOC))
		return false;

	return stream->flush();
}

/**
 * Reads a RGN marker (Region Of Interest)
 *
 * @param       codeStream                   JPEG 2000 code stream
 * @param       p_header_data   the data contained in the POC box.
 * @param       header_size   the size of the data contained in the POC marker.

 */
bool j2k_read_rgn(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	uint32_t comp_no, roi_sty;

	assert(p_header_data != nullptr);
	assert(codeStream != nullptr);

	auto image = codeStream->m_private_image;
	uint32_t nb_comp = image->numcomps;
	uint32_t comp_room = (nb_comp <= 256) ? 1 : 2;

	if (header_size != 2 + comp_room) {
		GROK_ERROR("Error reading RGN marker");
		return false;
	}

	auto tcp = codeStream->get_current_decode_tcp();

	/* Crgn */
	grk_read<uint32_t>(p_header_data, &comp_no, comp_room);
	p_header_data += comp_room;
	/* Srgn */
	grk_read<uint32_t>(p_header_data, &roi_sty, 1);
	if (roi_sty != 0) {
		GROK_WARN(
				"RGN marker RS value of %d is not supported by JPEG 2000 Part 1",
				roi_sty);
	}
	++p_header_data;

	/* testcase 3635.pdf.asan.77.2930 */
	if (comp_no >= nb_comp) {
		GROK_ERROR("bad component number in RGN (%d when there are only %d)",
				comp_no, nb_comp);
		return false;
	}

	/* SPrgn */
	grk_read<uint32_t>(p_header_data,
			(uint32_t*) (&(tcp->tccps[comp_no].roishift)), 1);
	++p_header_data;

	return true;
}

bool j2k_write_mct_data_group(CodeStream *codeStream, BufferedStream *stream) {
	uint32_t i;
	assert(codeStream != nullptr);
	assert(stream != nullptr);
	if (!j2k_write_cbd(codeStream, stream))
		return false;

	auto tcp = &(codeStream->m_cp.tcps[codeStream->m_tileProcessor->m_current_tile_index]);
	auto mct_record = tcp->m_mct_records;

	for (i = 0; i < tcp->m_nb_mct_records; ++i) {
		if (!j2k_write_mct_record(mct_record, stream))
			return false;
		++mct_record;
	}

	auto mcc_record = tcp->m_mcc_records;
	for (i = 0; i < tcp->m_nb_mcc_records; ++i) {
		if (!j2k_write_mcc_record(mcc_record, stream))
			return false;
		++mcc_record;
	}

	return j2k_write_mco(codeStream, stream);
}

bool j2k_write_all_coc(CodeStream *codeStream, BufferedStream *stream) {
	uint32_t compno;

	assert(codeStream != nullptr);
	assert(stream != nullptr);
	for (compno = 1; compno < codeStream->m_private_image->numcomps; ++compno) {
		/* cod is first component of first tile */
		if (!j2k_compare_coc(codeStream, 0, compno)) {
			if (!j2k_write_coc(codeStream, compno, stream))
				return false;
		}
	}

	return true;
}

bool j2k_write_all_qcc(CodeStream *codeStream, BufferedStream *stream) {
	uint32_t compno;

	assert(codeStream != nullptr);
	assert(stream != nullptr);
	for (compno = 1; compno < codeStream->m_private_image->numcomps; ++compno) {
		/* qcd is first component of first tile */
		if (!j2k_compare_qcc(codeStream, 0, compno)) {
			if (!j2k_write_qcc(codeStream, compno, stream))
				return false;
		}
	}
	return true;
}

bool j2k_write_regions(CodeStream *codeStream, BufferedStream *stream) {
	uint32_t compno;
	assert(codeStream != nullptr);
	assert(stream != nullptr);

	auto tccp = codeStream->m_cp.tcps->tccps;
	for (compno = 0; compno < codeStream->m_private_image->numcomps; ++compno) {
		if (tccp->roishift) {
			if (!j2k_write_rgn(codeStream, 0, compno,
					codeStream->m_private_image->numcomps, stream))
				return false;
		}
		++tccp;
	}

	return true;
}

bool j2k_write_epc(CodeStream *codeStream, BufferedStream *stream) {
	assert(codeStream != nullptr);
	assert(stream != nullptr);

	auto cstr_index = codeStream->cstr_index;
	if (cstr_index) {
		cstr_index->codestream_size = (uint64_t) stream->tell();
		/* The following adjustment is done to adjust the code stream size */
		/* if SOD is not at 0 in the buffer. Useful in case of JP2, where */
		/* the first bunch of bytes is not in the code stream              */
		cstr_index->codestream_size -= (uint64_t) cstr_index->main_head_start;

	}
	return true;
}

bool j2k_write_mct_record(grk_mct_data *p_mct_record, BufferedStream *stream) {
	uint32_t mct_size;
	uint32_t tmp;

	assert(stream != nullptr);

	mct_size = 10 + p_mct_record->m_data_size;

	/* MCT */
	if (!stream->write_short(J2K_MS_MCT))
		return false;
	/* Lmct */
	if (!stream->write_short((uint16_t) (mct_size - 2)))
		return false;
	/* Zmct */
	if (!stream->write_short(0))
		return false;
	/* only one marker atm */
	tmp = (p_mct_record->m_index & 0xff)
			| (uint32_t) (p_mct_record->m_array_type << 8)
			| (uint32_t) (p_mct_record->m_element_type << 10);

	if (!stream->write_short((uint16_t) tmp))
		return false;

	/* Ymct */
	if (!stream->write_short(0))
		return false;

	return stream->write_bytes(p_mct_record->m_data, p_mct_record->m_data_size);
}

/**
 * Reads a MCT marker (Multiple Component Transform)
 *
 * @param       codeStream                   JPEG 2000 code stream
 * @param       p_header_data   the data contained in the MCT box.
 * @param       header_size   the size of the data contained in the MCT marker.

 */
bool j2k_read_mct(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	uint32_t i;
	uint32_t tmp;
	uint32_t indix;

	assert(p_header_data != nullptr);
	assert(codeStream != nullptr);

	auto tcp = codeStream->get_current_decode_tcp();

	if (header_size < 2) {
		GROK_ERROR("Error reading MCT marker");
		return false;
	}

	/* first marker */
	/* Zmct */
	grk_read<uint32_t>(p_header_data, &tmp, 2);
	p_header_data += 2;
	if (tmp != 0) {
		GROK_WARN("Cannot take in charge mct data within multiple MCT records");
		return true;
	}

	if (header_size <= 6) {
		GROK_ERROR("Error reading MCT marker");
		return false;
	}

	/* Imct -> no need for other values, take the first,
	 * type is double with decorrelation x0000 1101 0000 0000*/
	grk_read<uint32_t>(p_header_data, &tmp, 2); /* Imct */
	p_header_data += 2;

	indix = tmp & 0xff;
	auto mct_data = tcp->m_mct_records;

	for (i = 0; i < tcp->m_nb_mct_records; ++i) {
		if (mct_data->m_index == indix)
			break;
		++mct_data;
	}

	bool newmct = false;
	// NOT FOUND
	if (i == tcp->m_nb_mct_records) {
		if (tcp->m_nb_mct_records == tcp->m_nb_max_mct_records) {
			grk_mct_data *new_mct_records;
			tcp->m_nb_max_mct_records += default_number_mct_records;

			new_mct_records = (grk_mct_data*) grk_realloc(tcp->m_mct_records,
					tcp->m_nb_max_mct_records * sizeof(grk_mct_data));
			if (!new_mct_records) {
				grok_free(tcp->m_mct_records);
				tcp->m_mct_records = nullptr;
				tcp->m_nb_max_mct_records = 0;
				tcp->m_nb_mct_records = 0;
				GROK_ERROR("Not enough memory to read MCT marker");
				return false;
			}

			/* Update m_mcc_records[].m_offset_array and m_decorrelation_array
			 * to point to the new addresses */
			if (new_mct_records != tcp->m_mct_records) {
				for (i = 0; i < tcp->m_nb_mcc_records; ++i) {
					grk_simple_mcc_decorrelation_data *mcc_record =
							&(tcp->m_mcc_records[i]);
					if (mcc_record->m_decorrelation_array) {
						mcc_record->m_decorrelation_array = new_mct_records
								+ (mcc_record->m_decorrelation_array
										- tcp->m_mct_records);
					}
					if (mcc_record->m_offset_array) {
						mcc_record->m_offset_array = new_mct_records
								+ (mcc_record->m_offset_array
										- tcp->m_mct_records);
					}
				}
			}

			tcp->m_mct_records = new_mct_records;
			mct_data = tcp->m_mct_records + tcp->m_nb_mct_records;
			memset(mct_data, 0,
					(tcp->m_nb_max_mct_records - tcp->m_nb_mct_records)
							* sizeof(grk_mct_data));
		}

		mct_data = tcp->m_mct_records + tcp->m_nb_mct_records;
		newmct = true;
	}

	if (mct_data->m_data) {
		grok_free(mct_data->m_data);
		mct_data->m_data = nullptr;
		mct_data->m_data_size = 0;
	}

	mct_data->m_index = indix;
	mct_data->m_array_type = (J2K_MCT_ARRAY_TYPE) ((tmp >> 8) & 3);
	mct_data->m_element_type = (J2K_MCT_ELEMENT_TYPE) ((tmp >> 10) & 3);

	/* Ymct */
	grk_read<uint32_t>(p_header_data, &tmp, 2);
	p_header_data += 2;
	if (tmp != 0) {
		GROK_WARN("Cannot take in charge multiple MCT markers");
		return true;
	}
	if (header_size < 6) {
		GROK_ERROR("Error reading MCT markers");
		return false;
	}
	header_size = (uint16_t) (header_size - 6);

	mct_data->m_data = (uint8_t*) grk_malloc(header_size);
	if (!mct_data->m_data) {
		GROK_ERROR("Error reading MCT marker");
		return false;
	}
	memcpy(mct_data->m_data, p_header_data, header_size);
	mct_data->m_data_size = header_size;
	if (newmct)
		++tcp->m_nb_mct_records;

	return true;
}

bool j2k_write_mcc_record(grk_simple_mcc_decorrelation_data *p_mcc_record,
		BufferedStream *stream) {
	uint32_t i;
	uint32_t mcc_size;
	uint32_t nb_bytes_for_comp;
	uint32_t mask;
	uint32_t tmcc;

	assert(stream != nullptr);

	if (p_mcc_record->m_nb_comps > 255) {
		nb_bytes_for_comp = 2;
		mask = 0x8000;
	} else {
		nb_bytes_for_comp = 1;
		mask = 0;
	}

	mcc_size = p_mcc_record->m_nb_comps * 2 * nb_bytes_for_comp + 19;

	/* MCC */
	if (!stream->write_short(J2K_MS_MCC))
		return false;
	/* Lmcc */
	if (!stream->write_short((uint16_t) (mcc_size - 2)))
		return false;
	/* first marker */
	/* Zmcc */
	if (!stream->write_short(0))
		return false;
	/* Imcc -> no need for other values, take the first */
	if (!stream->write_byte((uint8_t) p_mcc_record->m_index))
		return false;
	/* only one marker atm */
	/* Ymcc */
	if (!stream->write_short(0))
		return false;
	/* Qmcc -> number of collections -> 1 */
	if (!stream->write_short(1))
		return false;
	/* Xmcci type of component transformation -> array based decorrelation */
	if (!stream->write_byte(0x1))
		return false;
	/* Nmcci number of input components involved and size for each component offset = 8 bits */
	if (!stream->write_short((uint16_t) (p_mcc_record->m_nb_comps | mask)))
		return false;

	for (i = 0; i < p_mcc_record->m_nb_comps; ++i) {
		/* Cmccij Component offset*/
		if (nb_bytes_for_comp == 2) {
			if (!stream->write_short((uint16_t) i))
				return false;
		} else {
			if (!stream->write_byte((uint8_t) i))
				return false;
		}
	}

	/* Mmcci number of output components involved and size for each component offset = 8 bits */
	if (!stream->write_short((uint16_t) (p_mcc_record->m_nb_comps | mask)))
		return false;

	for (i = 0; i < p_mcc_record->m_nb_comps; ++i) {
		/* Wmccij Component offset*/
		if (nb_bytes_for_comp == 2) {
			if (!stream->write_short((uint16_t) i))
				return false;
		} else {
			if (!stream->write_byte((uint8_t) i))
				return false;
		}
	}

	tmcc = ((uint32_t) ((!p_mcc_record->m_is_irreversible) & 1U)) << 16;

	if (p_mcc_record->m_decorrelation_array)
		tmcc |= p_mcc_record->m_decorrelation_array->m_index;

	if (p_mcc_record->m_offset_array)
		tmcc |= ((p_mcc_record->m_offset_array->m_index) << 8);

	/* Tmcci : use MCT defined as number 1 and irreversible array based. */
	return stream->write_24(tmcc);
}

bool j2k_read_mcc(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	uint32_t i, j;
	uint32_t tmp;
	uint32_t indix;
	uint32_t nb_collections;
	uint32_t nb_comps;
	uint32_t nb_bytes_by_comp;

	assert(p_header_data != nullptr);
	assert(codeStream != nullptr);

	auto tcp = codeStream->get_current_decode_tcp();

	if (header_size < 2) {
		GROK_ERROR("Error reading MCC marker");
		return false;
	}

	/* first marker */
	/* Zmcc */
	grk_read<uint32_t>(p_header_data, &tmp, 2);
	p_header_data += 2;
	if (tmp != 0) {
		GROK_WARN("Cannot take in charge multiple data spanning");
		return true;
	}

	if (header_size < 7) {
		GROK_ERROR("Error reading MCC marker");
		return false;
	}

	grk_read<uint32_t>(p_header_data, &indix, 1); /* Imcc -> no need for other values, take the first */
	++p_header_data;

	auto mcc_record = tcp->m_mcc_records;

	for (i = 0; i < tcp->m_nb_mcc_records; ++i) {
		if (mcc_record->m_index == indix)
			break;
		++mcc_record;
	}

	/** NOT FOUND */
	bool newmcc = false;
	if (i == tcp->m_nb_mcc_records) {
		// resize tcp->m_nb_mcc_records if necessary
		if (tcp->m_nb_mcc_records == tcp->m_nb_max_mcc_records) {
			grk_simple_mcc_decorrelation_data *new_mcc_records;
			tcp->m_nb_max_mcc_records += default_number_mcc_records;

			new_mcc_records = (grk_simple_mcc_decorrelation_data*) grk_realloc(
					tcp->m_mcc_records,
					tcp->m_nb_max_mcc_records
							* sizeof(grk_simple_mcc_decorrelation_data));
			if (!new_mcc_records) {
				grok_free(tcp->m_mcc_records);
				tcp->m_mcc_records = nullptr;
				tcp->m_nb_max_mcc_records = 0;
				tcp->m_nb_mcc_records = 0;
				GROK_ERROR("Not enough memory to read MCC marker");
				return false;
			}
			tcp->m_mcc_records = new_mcc_records;
			mcc_record = tcp->m_mcc_records + tcp->m_nb_mcc_records;
			memset(mcc_record, 0,
					(tcp->m_nb_max_mcc_records - tcp->m_nb_mcc_records)
							* sizeof(grk_simple_mcc_decorrelation_data));
		}
		// set pointer to prospective new mcc record
		mcc_record = tcp->m_mcc_records + tcp->m_nb_mcc_records;
		newmcc = true;
	}
	mcc_record->m_index = indix;

	/* only one marker atm */
	/* Ymcc */
	grk_read<uint32_t>(p_header_data, &tmp, 2);
	p_header_data += 2;
	if (tmp != 0) {
		GROK_WARN("Cannot take in charge multiple data spanning");
		return true;
	}

	/* Qmcc -> number of collections -> 1 */
	grk_read<uint32_t>(p_header_data, &nb_collections, 2);
	p_header_data += 2;

	if (nb_collections > 1) {
		GROK_WARN("Cannot take in charge multiple collections");
		return true;
	}
	header_size = (uint16_t) (header_size - 7);

	for (i = 0; i < nb_collections; ++i) {
		if (header_size < 3) {
			GROK_ERROR("Error reading MCC marker");
			return false;
		}

		grk_read<uint32_t>(p_header_data, &tmp, 1); /* Xmcci type of component transformation -> array based decorrelation */
		++p_header_data;

		if (tmp != 1) {
			GROK_WARN(
					"Cannot take in charge collections other than array decorrelation");
			return true;
		}

		grk_read<uint32_t>(p_header_data, &nb_comps, 2);

		p_header_data += 2;
		header_size = (uint16_t) (header_size - 3);

		nb_bytes_by_comp = 1 + (nb_comps >> 15);
		mcc_record->m_nb_comps = nb_comps & 0x7fff;

		if (header_size < (nb_bytes_by_comp * mcc_record->m_nb_comps + 2)) {
			GROK_ERROR("Error reading MCC marker");
			return false;
		}

		header_size = (uint16_t) (header_size
				- (nb_bytes_by_comp * mcc_record->m_nb_comps + 2));

		for (j = 0; j < mcc_record->m_nb_comps; ++j) {
			/* Cmccij Component offset*/
			grk_read<uint32_t>(p_header_data, &tmp, nb_bytes_by_comp);
			p_header_data += nb_bytes_by_comp;

			if (tmp != j) {
				GROK_WARN(
						"Cannot take in charge collections with indix shuffle");
				return true;
			}
		}

		grk_read<uint32_t>(p_header_data, &nb_comps, 2);
		p_header_data += 2;

		nb_bytes_by_comp = 1 + (nb_comps >> 15);
		nb_comps &= 0x7fff;

		if (nb_comps != mcc_record->m_nb_comps) {
			GROK_WARN(
					"Cannot take in charge collections without same number of indices");
			return true;
		}

		if (header_size < (nb_bytes_by_comp * mcc_record->m_nb_comps + 3)) {
			GROK_ERROR("Error reading MCC marker");
			return false;
		}

		header_size = (uint16_t) (header_size
				- (nb_bytes_by_comp * mcc_record->m_nb_comps + 3));

		for (j = 0; j < mcc_record->m_nb_comps; ++j) {
			/* Wmccij Component offset*/
			grk_read<uint32_t>(p_header_data, &tmp, nb_bytes_by_comp);
			p_header_data += nb_bytes_by_comp;

			if (tmp != j) {
				GROK_WARN(
						"Cannot take in charge collections with indix shuffle");
				return true;
			}
		}
		/* Wmccij Component offset*/
		grk_read<uint32_t>(p_header_data, &tmp, 3);
		p_header_data += 3;

		mcc_record->m_is_irreversible = !((tmp >> 16) & 1);
		mcc_record->m_decorrelation_array = nullptr;
		mcc_record->m_offset_array = nullptr;

		indix = tmp & 0xff;
		if (indix != 0) {
			auto mct_data = tcp->m_mct_records;
			for (j = 0; j < tcp->m_nb_mct_records; ++j) {
				if (mct_data->m_index == indix) {
					mcc_record->m_decorrelation_array = mct_data;
					break;
				}
				++mct_data;
			}

			if (mcc_record->m_decorrelation_array == nullptr) {
				GROK_ERROR("Error reading MCC marker");
				return false;
			}
		}

		indix = (tmp >> 8) & 0xff;
		if (indix != 0) {
			auto mct_data = tcp->m_mct_records;
			for (j = 0; j < tcp->m_nb_mct_records; ++j) {
				if (mct_data->m_index == indix) {
					mcc_record->m_offset_array = mct_data;
					break;
				}
				++mct_data;
			}

			if (mcc_record->m_offset_array == nullptr) {
				GROK_ERROR("Error reading MCC marker");
				return false;
			}
		}
	}

	if (header_size != 0) {
		GROK_ERROR("Error reading MCC marker");
		return false;
	}

	// only increment mcc record count if we are working on a new mcc
	// and everything succeeded
	if (newmcc)
		++tcp->m_nb_mcc_records;

	return true;
}

bool j2k_write_mco(CodeStream *codeStream, BufferedStream *stream) {
	uint32_t mco_size;
	uint32_t i;

	assert(codeStream != nullptr);
	assert(stream != nullptr);

	auto tcp = &(codeStream->m_cp.tcps[codeStream->m_tileProcessor->m_current_tile_index]);
	mco_size = 5 + tcp->m_nb_mcc_records;

	/* MCO */
	if (!stream->write_short(J2K_MS_MCO))
		return false;

	/* Lmco */
	if (!stream->write_short((uint16_t) (mco_size - 2)))
		return false;

	/* Nmco : only one transform stage*/
	if (!stream->write_byte((uint8_t) tcp->m_nb_mcc_records))
		return false;

	auto mcc_record = tcp->m_mcc_records;
	for (i = 0; i < tcp->m_nb_mcc_records; ++i) {
		/* Imco -> use the mcc indicated by 1*/
		if (!stream->write_byte((uint8_t) mcc_record->m_index))
			return false;
		++mcc_record;
	}
	return true;
}

/**
 * Reads a MCO marker (Multiple Component Transform Ordering)
 *
 * @param       codeStream                   JPEG 2000 code stream
 * @param       p_header_data   the data contained in the MCO box.
 * @param       header_size   the size of the data contained in the MCO marker.

 */
bool j2k_read_mco(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	uint32_t tmp, i;
	uint32_t nb_stages;
	assert(p_header_data != nullptr);
	assert(codeStream != nullptr);

	auto image = codeStream->m_private_image;
	auto tcp = codeStream->get_current_decode_tcp();

	if (header_size < 1) {
		GROK_ERROR("Error reading MCO marker");
		return false;
	}
	/* Nmco : only one transform stage*/
	grk_read<uint32_t>(p_header_data, &nb_stages, 1);
	++p_header_data;

	if (nb_stages > 1) {
		GROK_WARN("Cannot take in charge multiple transformation stages.");
		return true;
	}

	if (header_size != nb_stages + 1) {
		GROK_WARN("Error reading MCO marker");
		return false;
	}

	auto tccp = tcp->tccps;

	for (i = 0; i < image->numcomps; ++i) {
		tccp->m_dc_level_shift = 0;
		++tccp;
	}
	grok_free(tcp->m_mct_decoding_matrix);
	tcp->m_mct_decoding_matrix = nullptr;

	for (i = 0; i < nb_stages; ++i) {
		grk_read<uint32_t>(p_header_data, &tmp, 1);
		++p_header_data;

		if (!j2k_add_mct(tcp, codeStream->m_private_image, tmp))
			return false;
	}

	return true;
}

bool j2k_add_mct(TileCodingParams *p_tcp, grk_image *p_image, uint32_t index) {
	uint32_t i;
	uint32_t data_size, mct_size, offset_size;
	uint32_t nb_elem;
	TileComponentCodingParams *tccp;

	assert(p_tcp != nullptr);

	auto mcc_record = p_tcp->m_mcc_records;

	for (i = 0; i < p_tcp->m_nb_mcc_records; ++i) {
		if (mcc_record->m_index == index)
			break;
	}

	if (i == p_tcp->m_nb_mcc_records) {
		/** element discarded **/
		return true;
	}

	if (mcc_record->m_nb_comps != p_image->numcomps) {
		/** do not support number of comps != image */
		return true;
	}

	auto deco_array = mcc_record->m_decorrelation_array;

	if (deco_array) {
		data_size = MCT_ELEMENT_SIZE[deco_array->m_element_type]
				* p_image->numcomps * p_image->numcomps;
		if (deco_array->m_data_size != data_size)
			return false;

		nb_elem = p_image->numcomps * p_image->numcomps;
		mct_size = nb_elem * (uint32_t) sizeof(float);
		p_tcp->m_mct_decoding_matrix = (float*) grk_malloc(mct_size);

		if (!p_tcp->m_mct_decoding_matrix)
			return false;

		j2k_mct_read_functions_to_float[deco_array->m_element_type](
				deco_array->m_data, p_tcp->m_mct_decoding_matrix, nb_elem);
	}

	auto offset_array = mcc_record->m_offset_array;

	if (offset_array) {
		data_size = MCT_ELEMENT_SIZE[offset_array->m_element_type]
				* p_image->numcomps;
		if (offset_array->m_data_size != data_size)
			return false;

		nb_elem = p_image->numcomps;
		offset_size = nb_elem * (uint32_t) sizeof(uint32_t);
		auto offset_data = (uint32_t*) grk_malloc(offset_size);

		if (!offset_data)
			return false;

		j2k_mct_read_functions_to_int32[offset_array->m_element_type](
				offset_array->m_data, offset_data, nb_elem);

		tccp = p_tcp->tccps;
		auto current_offset_data = offset_data;

		for (i = 0; i < p_image->numcomps; ++i) {
			tccp->m_dc_level_shift = (int32_t) *(current_offset_data++);
			++tccp;
		}

		grok_free(offset_data);
	}

	return true;
}

bool j2k_write_cbd(CodeStream *codeStream, BufferedStream *stream) {
	uint32_t i;
	assert(codeStream != nullptr);
	assert(stream != nullptr);

	auto image = codeStream->m_private_image;
	uint32_t cbd_size = 6 + codeStream->m_private_image->numcomps;

	/* CBD */
	if (!stream->write_short(J2K_MS_CBD))
		return false;

	/* L_CBD */
	if (!stream->write_short((uint16_t) (cbd_size - 2)))
		return false;

	/* Ncbd */
	if (!stream->write_short((uint16_t) image->numcomps))
		return false;

	auto comp = image->comps;

	for (i = 0; i < image->numcomps; ++i) {
		/* Component bit depth */
		if (!stream->write_byte(
				(uint8_t) ((comp->sgnd << 7) | (comp->prec - 1)))) {
			return false;
		}
		++comp;
	}
	return true;
}

/**
 * Reads a CBD marker (Component bit depth definition)
 *
 * @param       codeStream                   JPEG 2000 code stream
 * @param       p_header_data   the data contained in the CBD box.
 * @param       header_size   the size of the data contained in the CBD marker.

 */
bool j2k_read_cbd(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	uint32_t nb_comp, num_comp;
	uint32_t comp_def;
	uint32_t i;
	assert(p_header_data != nullptr);
	assert(codeStream != nullptr);

	num_comp = codeStream->m_private_image->numcomps;

	if (header_size != (codeStream->m_private_image->numcomps + 2)) {
		GROK_ERROR("Crror reading CBD marker");
		return false;
	}
	/* Ncbd */
	grk_read<uint32_t>(p_header_data, &nb_comp, 2);
	p_header_data += 2;

	if (nb_comp != num_comp) {
		GROK_ERROR("Crror reading CBD marker");
		return false;
	}

	auto comp = codeStream->m_private_image->comps;
	for (i = 0; i < num_comp; ++i) {
		/* Component bit depth */
		grk_read<uint32_t>(p_header_data, &comp_def, 1);
		++p_header_data;
		comp->sgnd = (comp_def >> 7) & 1;
		comp->prec = (comp_def & 0x7f) + 1;
		++comp;
	}

	return true;
}

/**
 * Reads a TLM marker (Tile Length Marker)
 *
 * @param       codeStream                   JPEG 2000 code stream
 * @param       p_header_data   the data contained in the TLM box.
 * @param       header_size   the size of the data contained in the TLM marker.

 */
bool j2k_read_tlm(CodeStream *codeStream, uint8_t *p_header_data,
		uint16_t header_size) {
	assert(codeStream);
	if (!codeStream->m_cp.tlm_markers)
		codeStream->m_cp.tlm_markers = new TileLengthMarkers();

	return codeStream->m_cp.tlm_markers->read(p_header_data, header_size);
}

bool j2k_write_tlm_begin(CodeStream *codeStream, BufferedStream *stream) {
	assert(codeStream != nullptr);
	assert(stream != nullptr);

	if (!codeStream->m_cp.tlm_markers)
		codeStream->m_cp.tlm_markers = new TileLengthMarkers(stream);

	return codeStream->m_cp.tlm_markers->write_begin(
			codeStream->m_specific_param.m_encoder.m_total_tile_parts);
}

void j2k_update_tlm(CodeStream *codeStream, uint32_t tile_part_size) {
	assert(codeStream->m_cp.tlm_markers);
	codeStream->m_cp.tlm_markers->write_update(
			codeStream->m_tileProcessor->m_current_tile_index, tile_part_size);
}

bool j2k_write_tlm_end(CodeStream *codeStream, BufferedStream *stream) {
	assert(codeStream);
	(void)stream;

	return codeStream->m_cp.tlm_markers->write_end();
}

uint32_t j2k_get_SPCod_SPCoc_size(CodeStream *codeStream, uint16_t tile_no,
		uint32_t comp_no) {
	assert(codeStream != nullptr);

	auto cp = &(codeStream->m_cp);
	auto tcp = &cp->tcps[tile_no];
	auto tccp = &tcp->tccps[comp_no];

	assert(tile_no < (cp->t_grid_width * cp->t_grid_height));
	assert(comp_no < codeStream->m_private_image->numcomps);

	uint32_t rc = SPCod_SPCoc_len;
	if (tccp->csty & J2K_CCP_CSTY_PRT)
		rc += tccp->numresolutions;

	return rc;
}

bool j2k_compare_SPCod_SPCoc(CodeStream *codeStream, uint16_t tile_no,
		uint32_t first_comp_no, uint32_t second_comp_no) {
	assert(codeStream != nullptr);

	auto cp = &(codeStream->m_cp);
	auto tcp = &cp->tcps[tile_no];
	auto tccp0 = &tcp->tccps[first_comp_no];
	auto tccp1 = &tcp->tccps[second_comp_no];

	if (tccp0->numresolutions != tccp1->numresolutions)
		return false;
	if (tccp0->cblkw != tccp1->cblkw)
		return false;
	if (tccp0->cblkh != tccp1->cblkh)
		return false;
	if (tccp0->cblk_sty != tccp1->cblk_sty)
		return false;
	if (tccp0->qmfbid != tccp1->qmfbid)
		return false;
	if ((tccp0->csty & J2K_CCP_CSTY_PRT) != (tccp1->csty & J2K_CCP_CSTY_PRT))
		return false;
	for (uint32_t i = 0U; i < tccp0->numresolutions; ++i) {
		if (tccp0->prcw[i] != tccp1->prcw[i])
			return false;
		if (tccp0->prch[i] != tccp1->prch[i])
			return false;
	}

	return true;
}

bool j2k_write_SPCod_SPCoc(CodeStream *codeStream, uint16_t tile_no,
		uint32_t comp_no, BufferedStream *stream) {
	assert(codeStream != nullptr);

	auto cp = &(codeStream->m_cp);
	auto tcp = &cp->tcps[tile_no];
	auto tccp = &tcp->tccps[comp_no];

	assert(tile_no < (cp->t_grid_width * cp->t_grid_height));
	assert(comp_no < (codeStream->m_private_image->numcomps));

	/* SPcoc (D) */
	if (!stream->write_byte((uint8_t) (tccp->numresolutions - 1)))
		return false;
	/* SPcoc (E) */
	if (!stream->write_byte((uint8_t) (tccp->cblkw - 2)))
		return false;
	/* SPcoc (F) */
	if (!stream->write_byte((uint8_t) (tccp->cblkh - 2)))
		return false;
	/* SPcoc (G) */
	if (!stream->write_byte(tccp->cblk_sty))
		return false;
	/* SPcoc (H) */
	if (!stream->write_byte((uint8_t) tccp->qmfbid))
		return false;

	if (tccp->csty & J2K_CCP_CSTY_PRT) {
		for (uint32_t i = 0; i < tccp->numresolutions; ++i) {
			/* SPcoc (I_i) */
			if (!stream->write_byte(
					(uint8_t) (tccp->prcw[i] + (tccp->prch[i] << 4)))) {
				return false;
			}
		}
	}

	return true;
}

bool j2k_read_SPCod_SPCoc(CodeStream *codeStream, uint32_t compno,
		uint8_t *p_header_data, uint16_t *header_size) {
	uint32_t i, tmp;
	assert(codeStream != nullptr);
	assert(p_header_data != nullptr);
	assert(compno < codeStream->m_private_image->numcomps);

	if (compno >= codeStream->m_private_image->numcomps)
		return false;

	auto cp = &(codeStream->m_cp);
	auto tcp = codeStream->get_current_decode_tcp();
	auto tccp = &tcp->tccps[compno];
	auto current_ptr = p_header_data;

	/* make sure room is sufficient */
	if (*header_size < SPCod_SPCoc_len) {
		GROK_ERROR("Error reading SPCod SPCoc element");
		return false;
	}
	/* SPcox (D) */
	grk_read<uint32_t>(current_ptr, &tccp->numresolutions, 1);
	++tccp->numresolutions;
	if (tccp->numresolutions > GRK_J2K_MAXRLVLS) {
		GROK_ERROR("Number of resolutions %d is greater than"
				" maximum allowed number %d", tccp->numresolutions,
		GRK_J2K_MAXRLVLS);
		return false;
	}
	++current_ptr;
	if (codeStream->m_cp.ccap && !tcp->isHT) {
		tcp->isHT = true;
		tcp->qcd.generate(tccp->numgbits, tccp->numresolutions - 1,
				tccp->qmfbid == 1, codeStream->m_private_image->comps[compno].prec,
				tcp->mct > 0, codeStream->m_private_image->comps[compno].sgnd);
		tcp->qcd.push(tccp->stepsizes, tccp->qmfbid == 1);
	}

	/* If user wants to remove more resolutions than the code stream contains, return error */
	if (cp->m_coding_params.m_dec.m_reduce >= tccp->numresolutions) {
		GROK_ERROR("Error decoding component %d.\nThe number of resolutions"
				" to remove is higher than the number "
				"of resolutions of this component\n"
				"Modify the cp_reduce parameter.\n", compno);
		codeStream->m_specific_param.m_decoder.m_state |= 0x8000;/* FIXME J2K_DEC_STATE_ERR;*/
		return false;
	}
	/* SPcoc (E) */
	grk_read<uint32_t>(current_ptr, &tccp->cblkw, 1);
	++current_ptr;
	tccp->cblkw += 2;
	/* SPcoc (F) */
	grk_read<uint32_t>(current_ptr, &tccp->cblkh, 1);
	++current_ptr;
	tccp->cblkh += 2;

	if ((tccp->cblkw > 10) || (tccp->cblkh > 10)
			|| ((tccp->cblkw + tccp->cblkh) > 12)) {
		GROK_ERROR("Error reading SPCod SPCoc element,"
				" Invalid cblkw/cblkh combination");
		return false;
	}

	/* SPcoc (G) */
	tccp->cblk_sty = *current_ptr++;
	/* SPcoc (H) */
	tccp->qmfbid = *current_ptr++;
	if (tccp->qmfbid > 1) {
		GROK_ERROR("Invalid qmfbid : %d. "
				"Should be either 0 or 1", tccp->qmfbid);
		return false;
	}
	*header_size = (uint16_t) (*header_size - SPCod_SPCoc_len);

	/* use custom precinct size ? */
	if (tccp->csty & J2K_CCP_CSTY_PRT) {
		if (*header_size < tccp->numresolutions) {
			GROK_ERROR("Error reading SPCod SPCoc element");
			return false;
		}

		for (i = 0; i < tccp->numresolutions; ++i) {
			/* SPcoc (I_i) */
			grk_read<uint32_t>(current_ptr, &tmp, 1);
			++current_ptr;
			/* Precinct exponent 0 is only allowed for lowest resolution level (Table A.21) */
			if ((i != 0) && (((tmp & 0xf) == 0) || ((tmp >> 4) == 0))) {
				GROK_ERROR("Invalid precinct size");
				return false;
			}
			tccp->prcw[i] = tmp & 0xf;
			tccp->prch[i] = tmp >> 4;
		}

		*header_size = (uint16_t) (*header_size - tccp->numresolutions);
	} else {
		/* set default size for the precinct width and height */
		for (i = 0; i < tccp->numresolutions; ++i) {
			tccp->prcw[i] = 15;
			tccp->prch[i] = 15;
		}
	}

	return true;
}

uint32_t j2k_get_SQcd_SQcc_size(CodeStream *codeStream, uint16_t tile_no,
		uint32_t comp_no) {
	assert(codeStream != nullptr);

	auto cp = &(codeStream->m_cp);
	auto tcp = &cp->tcps[tile_no];
	auto tccp = &tcp->tccps[comp_no];

	return tccp->quant.get_SQcd_SQcc_size(codeStream, tile_no, comp_no);
}

bool j2k_compare_SQcd_SQcc(CodeStream *codeStream, uint16_t tile_no,
		uint32_t first_comp_no, uint32_t second_comp_no) {
	assert(codeStream != nullptr);

	auto cp = &(codeStream->m_cp);
	auto tcp = &cp->tcps[tile_no];
	auto tccp0 = &tcp->tccps[first_comp_no];

	return tccp0->quant.compare_SQcd_SQcc(codeStream, tile_no, first_comp_no,
			second_comp_no);
}

bool j2k_write_SQcd_SQcc(CodeStream *codeStream, uint16_t tile_no, uint32_t comp_no,
		BufferedStream *stream) {
	assert(codeStream != nullptr);

	auto cp = &(codeStream->m_cp);
	auto tcp = &cp->tcps[tile_no];
	auto tccp = &tcp->tccps[comp_no];
	return tccp->quant.write_SQcd_SQcc(codeStream, tile_no, comp_no, stream);
}

bool j2k_read_SQcd_SQcc(bool fromQCC, CodeStream *codeStream, uint32_t comp_no,
		uint8_t *p_header_data, uint16_t *header_size) {
	assert(codeStream != nullptr);
	assert(p_header_data != nullptr);
	assert(comp_no < codeStream->m_private_image->numcomps);
	auto tcp = codeStream->get_current_decode_tcp();
	auto tccp = tcp->tccps + comp_no;
	return tccp->quant.read_SQcd_SQcc(fromQCC, codeStream, comp_no, p_header_data,
			header_size);
}

}
