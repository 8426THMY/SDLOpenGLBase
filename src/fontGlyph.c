#include "fontGlyph.h"


#include <stdlib.h>

#include "utilString.h"
#include "utilFile.h"

#include "memoryManager.h"


fontGlyph *fontGlyphArrayLoad(const char *glyphPath){
	fontGlyph *glyphs = NULL;

	// Load the glyph offsets!
	FILE *glyphFile = fopen(glyphPath, "r");
	if(glyphFile != NULL){
		char *endPos = NULL;

		size_t numGlyphs = invalidValue(numGlyphs);

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		while((line = fileReadLine(glyphFile, &lineBuffer[0], &lineLength)) != NULL){
			// If we haven't read the character count, try and read it!
			if(valueIsInvalid(numGlyphs)){
				numGlyphs = strtoul(line, &endPos, 10);
				// If we didn't read a number, return numGlyphs to the invalid value.
				if(endPos == line){
					numGlyphs = invalidValue(numGlyphs);

				// Otherwise, allocate memory for our glyphs!
				}else{
					glyphs = memoryManagerGlobalAlloc(sizeof(*glyphs) * numGlyphs);
				}
			}else if(line[0] != '\0'){
				size_t tokenLength;
				char *token = line;

				// Load the glyph data from the file!
				const size_t char_id = (token = getDelimitedString(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtoul(token, NULL, 10));
				const size_t char_index = (token += tokenLength + 1, token = getDelimitedString(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtoul(token, NULL, 10));
				const uint32_t char_char = (token += tokenLength + 1, token = getDelimitedString(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', 1234);
				const float char_width = (token += tokenLength + 1, token = getDelimitedString(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtof(token, NULL));
				const float char_height = (token += tokenLength + 1, token = getDelimitedString(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtof(token, NULL));
				const float char_xoffset = (token += tokenLength + 1, token = getDelimitedString(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtof(token, NULL));
				const float char_yoffset = (token += tokenLength + 1, token = getDelimitedString(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtof(token, NULL));
				const float char_xadvance = (token += tokenLength + 1, token = getDelimitedString(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtof(token, NULL));
				const byte_t char_chnl = (token += tokenLength + 1, token = getDelimitedString(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtoul(token, NULL, 10));
				const float char_x = (token += tokenLength + 1, token = getDelimitedString(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtof(token, NULL));
				const float char_y = (token += tokenLength + 1, token = getDelimitedString(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtof(token, NULL));
				const byte_t char_page = (token += tokenLength + 1, token = getDelimitedString(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', strtoul(token, NULL, 10));
				const uint32_t info_charset = (token += tokenLength + 1, token = getDelimitedString(token, lineLength, '"', &tokenLength), token[tokenLength] = '\0', 1234);

				const fontGlyph newGlyph = {
					.uvOffsets.x = char_x,
					.uvOffsets.y = char_y,
					.uvOffsets.w = char_width,
					.uvOffsets.h = char_height,

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
	fontGlyph *glyphs = NULL;

	// Load the glyph offsets!
	FILE *glyphFile = fopen(glyphPath, "r");
	if(glyphFile != NULL){
		char *endPos = NULL;

		size_t numGlyphs = invalidValue(numGlyphs);

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;

		while((line = fileReadLine(glyphFile, &lineBuffer[0], &lineLength)) != NULL){
			// If we haven't read the character count, try and read it!
			if(valueIsInvalid(numGlyphs)){
				numGlyphs = strtoul(line, &endPos, 10);
				// If we didn't read a number, return numGlyphs to the invalid value.
				if(endPos == line){
					numGlyphs = invalidValue(numGlyphs);
				}
			}else{
				const size_t curID = strtoul(line, &endPos, 10);
				// If we could load the character identifier,
				// we can try and load the rest of the glyph.
				// Otherwise, the line was probably empty.
				if(endPos != line){
					const fontGlyph curGlyph = {
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
						"%u: (%f, %f, %f, %f), (%f, %f), %f\n",
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