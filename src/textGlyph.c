#include "textGlyph.h"


#include <stdlib.h>

#include "utilString.h"
#include "utilFile.h"

#include "memoryManager.h"


textGlyph *textGlyphArrayLoad(const char *const restrict glyphPath, const texture **const restrict atlasArray){
	textGlyph *glyphs = NULL;

	// Load the glyph offsets!
	FILE *const glyphFile = fopen(glyphPath, "r");
	if(glyphFile != NULL){
		char *endPos = NULL;

		size_t lastIndex = valueInvalid(size_t);

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		while((line = fileReadLine(glyphFile, &lineBuffer[0], &lineLength)) != NULL){
			// If we haven't read the character count, try and read it!
			if(valueIsInvalid(lastIndex, size_t)){
				lastIndex = strtoul(line, &endPos, 10);
				// If we didn't read a number, return numGlyphs to the invalid value.
				if(endPos == line){
					lastIndex = valueInvalid(size_t);

				// Otherwise, allocate memory for our glyphs!
				}else{
					glyphs = memoryManagerGlobalAlloc(sizeof(*glyphs) * (lastIndex + 1));
					if(glyphs == NULL){
						/** MALLOC FAILED **/
					}
				}
			}else if(lineLength > 0){
				size_t tokenLength;
				char *token = line;

				// Load the glyph data from the file!
				/** Unused. **/
				const size_t char_id    = (tokenLength = stringDelimited(token, lineLength, '\"', (const char **)&token), token[tokenLength] = '\0', strtoul(token, NULL, 10));
				const size_t char_index = (tokenLength = stringDelimited(&token[tokenLength + 1], lineLength, '\"', (const char **)&token), token[tokenLength] = '\0', strtoul(token, NULL, 10));
				/** Unused, not functional at the moment. **/
				const uint32_t char_char  = (tokenLength = stringDelimited(&token[tokenLength + 1], lineLength, '\"', (const char **)&token), token[tokenLength] = '\0', 1234);
				const float char_width    = (tokenLength = stringDelimited(&token[tokenLength + 1], lineLength, '\"', (const char **)&token), token[tokenLength] = '\0', strtof(token, NULL));
				const float char_height   = (tokenLength = stringDelimited(&token[tokenLength + 1], lineLength, '\"', (const char **)&token), token[tokenLength] = '\0', strtof(token, NULL));
				const float char_xoffset  = (tokenLength = stringDelimited(&token[tokenLength + 1], lineLength, '\"', (const char **)&token), token[tokenLength] = '\0', strtof(token, NULL));
				const float char_yoffset  = (tokenLength = stringDelimited(&token[tokenLength + 1], lineLength, '\"', (const char **)&token), token[tokenLength] = '\0', strtof(token, NULL));
				const float char_xadvance = (tokenLength = stringDelimited(&token[tokenLength + 1], lineLength, '\"', (const char **)&token), token[tokenLength] = '\0', strtof(token, NULL));
				/** Unused. **/
				const byte_t char_chnl = (tokenLength = stringDelimited(&token[tokenLength + 1], lineLength, '\"', (const char **)&token), token[tokenLength] = '\0', strtoul(token, NULL, 10));
				const float char_x     = (tokenLength = stringDelimited(&token[tokenLength + 1], lineLength, '\"', (const char **)&token), token[tokenLength] = '\0', strtof(token, NULL));
				const float char_y     = (tokenLength = stringDelimited(&token[tokenLength + 1], lineLength, '\"', (const char **)&token), token[tokenLength] = '\0', strtof(token, NULL));
				const byte_t char_page = (tokenLength = stringDelimited(&token[tokenLength + 1], lineLength, '\"', (const char **)&token), token[tokenLength] = '\0', strtoul(token, NULL, 10));
				/** Unused, not functional at the moment. **/
				const uint32_t info_charset = (tokenLength = stringDelimited(&token[tokenLength + 1], lineLength, '\"', (const char **)&token), token[tokenLength] = '\0', 1234);

				const texture *const atlas = atlasArray[char_page];
				const float atlasInvWidth  = 1.f/atlas->width;
				const float atlasInvHeight = 1.f/atlas->height;

				const textGlyph newGlyph = {
					.atlas = atlas,
					.uvOffsets.x = char_x * atlasInvWidth,
					.uvOffsets.y = char_y * atlasInvHeight,
					.uvOffsets.w = char_width * atlasInvWidth,
					.uvOffsets.h = char_height * atlasInvHeight,

					.kerningX = char_xoffset,
					.kerningY = char_yoffset,
					.advanceX = char_xadvance
				};
				// Add the new glyph to its correct position in our array of glyphs!
				glyphs[char_index] = newGlyph;
			}
		}

		fclose(glyphFile);
	}else{
		printf(
			"Unable to open glyph offsets file!\n"
			"Path: %s\n",
			glyphPath
		);
	}


	return(glyphs);
	#if 0
	textGlyph *glyphs = NULL;

	// Load the glyph offsets!
	FILE *const glyphFile = fopen(glyphPath, "r");
	if(glyphFile != NULL){
		char *endPos = NULL;

		size_t numGlyphs = valueInvalid(size_t);

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		while((line = fileReadLine(glyphFile, &lineBuffer[0], &lineLength)) != NULL){
			// If we haven't read the character count, try and read it!
			if(valueIsInvalid(numGlyphs, size_t)){
				numGlyphs = strtoul(line, &endPos, 10);
				// If we didn't read a number, return numGlyphs to the invalid value.
				if(endPos == line){
					numGlyphs = valueInvalid(size_t);
				}
			}else{
				const size_t curID = strtoul(line, &endPos, 10);
				// If we could load the character identifier,
				// we can try and load the rest of the glyph.
				// Otherwise, the line was probably empty.
				if(endPos != line){
					const textGlyph curGlyph = {
						.uvOffsets = {
							.x = strtof(endPos + 1, &endPos),
							.y = strtof(endPos + 1, &endPos),
							.w = strtof(endPos + 1, &endPos),
							.h = strtof(endPos + 1, &endPos)
						},

						.kerningX = strtof(endPos + 1, &endPos),
						.kerningY = strtof(endPos + 1, &endPos),
						.advanceX = strtof(endPos + 1, NULL)
					};
					printf(
						PRINTF_SIZE_T": (%f, %f, %f, %f), (%f, %f), %f\n",
						curID,
						curGlyph.uvOffsets.x, curGlyph.uvOffsets.y,
						curGlyph.uvOffsets.w, curGlyph.uvOffsets.h,
						curGlyph.kerningX, curGlyph.kerningX,
						curGlyph.advanceX
					);
				}
			}
		}
	}else{
		printf(
			"Unable to open glyph offsets file!\n"
			"Path: %s\n",
			glyphPath
		);
	}


	return(glyphs);
	#endif
}