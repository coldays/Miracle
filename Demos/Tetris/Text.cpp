#include "Text.h"
#include "ttf2mesh/ttf2mesh.h"

std::unordered_map<char, size_t> CharToMeshIndex;

static ttf_t* font = NULL;

bool InitializeTextLibrary() {
	// list all system fonts by filename mask:

	ttf_t** list = ttf_list_system_fonts("consola");
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
		if (ttf_glyph2mesh(&font->glyphs[index], &out, 128, TTF_FEATURE_IGN_ERR) != TTF_DONE)
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
		if (c != ' ') {
			CreateGlyphEntity(pos, scale, color, c);
		}
		pos.x += 0.6 * scale;
	}
}

void Text::Clear() {
	for (EntityContext& ec : m_entites) {
		ec.destroyEntity();
	}
	m_entites.clear();
}

Text::Text(){}

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

void Text::Show() {
	for (EntityContext& ec : m_entites) {
		ec.getAppearance().setVisible(true);
	}
}

void Text::Hide() {
	for (EntityContext& ec : m_entites) {
		ec.getAppearance().setVisible(false);
	}
}