/*
 * libalign -- alignment utilities
 *
 * Copyright (c) 2003-2004, Li Heng <liheng@genomics.org.cn>
 *                                  <lihengsci@yahoo.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "seq.h"

/* Read sequences from file "fp" in FASTA format. Sequence will be saved
 * in "seq", sequence ID in "locus", and comment saved in "comment",
 * provided "comment != 0". Sequence length will be returned. If -1 is
 * returned, no sequence is left in the file. */
int read_fasta(FILE *fp, seq_t *seq, char *locus, char *comment)
{
	int c, l, max;
	char *p;
	
	c = 0;
	while (!feof(fp) && fgetc(fp) != '>');
	if (feof(fp)) return -1;
	p = locus;
	while (!feof(fp) && (c = fgetc(fp)) != ' ' && c != '\t' && c != '\n')
		if (c != '\r') *p++ = c;
	*p = '\0';
	if (comment) {
		p = comment;
		if (c != '\n') {
			while (!feof(fp) && ((c = fgetc(fp)) == ' ' || c == '\t'));
			if (c != '\n') {
				*p++ = c;
				while (!feof(fp) && (c = fgetc(fp)) != '\n')
					if (c != '\r') *p++ = c;
			}
		}
		*p = '\0';
	} else if (c != '\n') while (!feof(fp) && fgetc(fp) != '\n');
	l = 0; max = seq->m;
	while (!feof(fp) && (c = fgetc(fp)) != '>') {
		if (isalpha(c) || c == '-' || c == '.') {
			if (l + 1 >= max) {
				max += SEQ_BLOCK_SIZE;
				seq->s = (char*)MYREALLOC(seq->s, sizeof(char) * max);
			}
			seq->s[l++] = (char)c;
		}
	}
	if (c == '>') ungetc(c,fp);
	seq->s[l] = 0;
	seq->m = max; seq->l = l;
	return l;
}
int read_fasta_str(char *buffer, seq_t *seq, char *locus, char *comment, char **ptr)
{
	int c, l, max;
	char *p;
	char *q;
	
	c = 0; q = buffer;
	while (*q && *q++ != '>');
	if (*q == 0) return -1;
	p = locus;
	while (*q && (c = *q++) != ' ' && c != '\t' && c != '\n')
		if (c != '\r') *p++ = c;
	*p = '\0';
	if (comment) {
		p = comment;
		if (c != '\n') {
			while (*q && ((c = *q++) == ' ' || c == '\t'));
			if (c != '\n') {
				*p++ = c;
				while (*q && (c = *q++) != '\n')
					if (c != '\r') *p++ = c;
			}
		}
		*p = '\0';
	} else if (c != '\n') while (*q && *q++ != '\n');
	l = 0; max = seq->m;
	while (*q && (c = *q++) != '>') {
		if (isalpha(c) || c == '-' || c == '.') {
			if (l + 1 >= max) {
				max += SEQ_BLOCK_SIZE;
				seq->s = (char*)MYREALLOC(seq->s, sizeof(char) * max);
			}
			seq->s[l++] = (char)c;
		}
	}
	if (c == '>') --q;
	seq->s[l] = 0;
	seq->m = max; seq->l = l;
	*ptr = q;
	return l;
}
/* Read quality from file "fp" in FASTA format. Quality will be saved
 * in "seq", sequence ID in "locus", and comment saved in "comment",
 * provided "comment != 0". Sequence length will be returned. If -1 is
 * returned, no sequence is left in the file. */
int read_qual(FILE *fp, seq_t *seq, char *locus, char *comment)
{
	int c, l, max;
	char *p, *q, tmp[8];

	c = 0;
	while (!feof(fp) && fgetc(fp) != '>');
	if (feof(fp)) return -1;
	p = locus;
	while (!feof(fp) && (c = fgetc(fp)) != ' ' && c != '\t' && c != '\n')
		if (c != '\r') *p++ = c;
	*p = '\0';
	if (comment) {
		p = comment;
		if (c != '\n') {
			while (!feof(fp) && ((c = fgetc(fp)) == ' ' || c == '\t'));
			if (c != '\n') {
				*p++ = c;
				while (!feof(fp) && (c = fgetc(fp)) != '\n')
					if (c != '\r') *p++ = c;
			}
		}
		*p = '\0';
	}
	if (c != '\n') while (!feof(fp) && fgetc(fp) != '\n');
	l = 0; max = seq->m;
	q = tmp;
	while (!feof(fp) && (c = fgetc(fp)) != '>') {
		if (isdigit(c)) *q++ = c;
		else if (q != tmp && (c == '\t' || c == ' ' || c == '\n')) {
			*q = '\0';
			if (l + 1 >= max) {
				max += SEQ_BLOCK_SIZE;
				seq->s = (char*)MYREALLOC(seq->s, sizeof(char) * max);
			}
			seq->s[l++] = (char)atoi(tmp);
			q = tmp;
		}
	}
	if (c == '>') ungetc(c, fp);
	seq->s[l] = 0;
	seq->m = max; seq->l = l;
	return l;
}
