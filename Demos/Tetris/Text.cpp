#include "Text.h"
#include "ttf2mesh/ttf2mesh.h"

std::unordered_map<char, size_t> CharToMeshIndex;

static ttf_t* font = NULL;

bool InitializeTextLibrary() {
	// list all system fonts by filename mask:

	ttf_t** list = ttf_list_system_fonts("Cascadia*");
	if (list == NULL) return false; // no memory in system
	if (list[0] == NULL) return false; // no fonts were found

	// load the first font from the list

	ttf_load_from_file(list[0]->filename, &font, false);
	ttf_free_list(list);
	if (font == NULL) return false;

	printf("font \"%s\" loaded\n", font->names.full_name);
	return true;
}

void AddTextMeshes(std::vector<Mesh>& meshes) {
	if (!InitializeTextLibrary()) {

	}

	for (char c = 33; c < 127; c++) {
		// find a glyph in the font file

		int index = ttf_find_glyph(font, c);
		if (index < 0) {
			Logger::error("Error finding glyph for " + c);
			exit(-1);
		}

		// make mesh object from the glyph

		ttf_mesh_t* out;
		if (ttf_glyph2mesh(&font->glyphs[index], &out, TTF_QUALITY_HIGH, TTF_FEATURE_IGN_ERR) != TTF_DONE)
			continue;

		ttf_glyph_t* glyph = &font->glyphs[index];

		Mesh mesh = {};

		for (int i = 0; i < out->nvert; i++) {
			auto vertex = out->vert[i];
			mesh.vertices.push_back(Vertex{
				.position = Vector3{.x = vertex.x, .y = vertex.y, .z = 0.0f }
			});
		}

		for (int i = 0; i < out->nfaces; i++) {
			auto face = out->faces[i];
			mesh.faces.push_back(Face{
				.indices = { (uint32_t)face.v1, (uint32_t)face.v2, (uint32_t)face.v3 }
			});
		}

		CharToMeshIndex[c] = meshes.size();
		meshes.push_back(mesh);

		ttf_free_mesh(out);
	}
}

void Text::CreateGlyphEntity(Vector2 pos, float scale, ColorRgb color, char c) {
	if (!CharToMeshIndex.contains(c)) {
		Logger::error(std::string("Could not find mesh for ") + c);
		return;
	}
	size_t index = CharToMeshIndex[c];
	m_entites.push_back(CurrentScene::createAndGetEntity(EntityConfig{
		.transformConfig = TransformConfig{
			.translation = Vector3{
				.x = pos.x,
				.y = pos.y,
				.z = 0},
			.scale = Vector3 { .x = scale, .y = scale }
		},
		.appearanceConfig = AppearanceConfig{
			.meshIndex = index,
			.color = color
		}
		}));
}

void Text::CreateText(Vector2 pos, float scale, ColorRgb color, std::string text) {

	for (char c : text) {
		// Outside of ascii range, use ? instead
		if (c < 32 || c > 126) {
			c = '?';
		}
		CreateGlyphEntity(pos, scale, color, c);
		pos.x += 0.8 * scale;
	}
}

void Text::Clear() {
	for (EntityContext& ec : m_entites) {
		ec.destroyEntity();
	}
	m_entites.clear();
}

Text::Text(Vector2 pos, float scale, ColorRgb color, std::string text) {
	m_pos = pos;
	m_scale = scale;
	m_color = color;

	CreateText(pos, scale, color, text);
}

void Text::ChangeText(std::string text) {
	Clear();
	CreateText(m_pos, m_scale, m_color, text);
}

void Text::Move(Vector2 dest) {

}

//class Text {
//private:
//	std::vector<EntityContext> m_entites;
//
//	EntityContext CreateBlock(Vector2 pos, Vector2 scale, ColorRgb color) {
//		return CurrentScene::createAndGetEntity(EntityConfig{
//			.transformConfig = TransformConfig{
//				.translation = Vector3{
//					.x = pos.x,
//					.y = pos.y,
//					.z = 0
//				},
//				.scale = Vector3{
//					.x = scale.x,
//					.y = scale.y,
//					.z = 0
//				}
//			},
//			.appearanceConfig = AppearanceConfig{
//				.meshIndex = 0,
//				.color = color
//			}
//		});
//	}
//
//
//	void CreateQuestionMark(Vector2 pos, float scale, ColorRgb color) {
//		m_entites.push_back(CreateBlock(Vector2{ .x = pos.x - 0.5f, .y = pos.y - 0.25f }, Vector2{ .x = 0.5f, .y = 1.0f }, color));
//		m_entites.push_back(CreateBlock(Vector2{ .x = pos.x - 0.25f, .y = pos.y - 0.25f }, Vector2{ .x = 1.0f, .y = 0.5f }, color));
//		m_entites.push_back(CreateBlock(Vector2{ .x = pos.x + 0.5f, .y = pos.y - 0.25f }, Vector2{ .x = 1.0f, .y = 0.5f }, color));
//	}
//
//	void AddChar(Vector2 pos, float scale, ColorRgb color, char c) {
//		switch (c) {
//			case '?':
//			default:
//				CreateQuestionMark(pos, scale, color);
//				break;
//		}
//	}
//
//public:
//	Text(Vector2 pos, float scale, ColorRgb color, std::string text) {
//
//		Vector2 currentPos = pos;
//		for (char c : text) {
//			AddChar(currentPos, scale, color, c);
//			currentPos.x += 10 * scale;
//		}
//	}
//
//	void ChangeText(std::string text) {
//
//	}
//
//	void Move(Vector2 dest) {
//
//	}
//
//
//};