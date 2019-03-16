///////////////////////////////////////////////////////////////////////////////
// Icosphere.h
// ===========
// Polyhedron subdividing icosahedron (20 tris) by N-times iteration
// The icosphere with N=1 (default) has 80 triangles by subdividing a triangle
// of icosahedron into 4 triangles. If N=0, it is identical to icosahedron.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2018-07-23
// UPDATED: 2018-07-27
///////////////////////////////////////////////////////////////////////////////
// Tanguy Fautre: 2019-02-28: Original from http://www.songho.ca/opengl/gl_sphere.html
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <map>

namespace Assets
{
	class Icosphere final
	{
	public:

		Icosphere(const Icosphere&) = delete;
		Icosphere& operator = (const Icosphere&) = delete;
		Icosphere& operator = (Icosphere&&) = delete;

		Icosphere(Icosphere&&) = default;
		Icosphere(float radius = 1.0f, int subdivision = 1, bool smooth = false);
		~Icosphere() = default;

		// getters/setters
		float Radius() const { return radius_; }
		int Subdivision() const { return subdivision_; }
		bool Smooth() const { return smooth_; }

		// for vertex data
		size_t VertexCount() const { return vertices_.size() / 3; }
		size_t NormalCount() const { return normals_.size() / 3; }
		size_t TexCoordCount() const { return texCoords_.size() / 2; }
		size_t IndexCount() const { return indices_.size(); }
		size_t LineIndexCount() const { return lineIndices_.size(); }
		size_t TriangleCount() const { return IndexCount() / 3; }

		const std::vector<float>& Vertices() const { return vertices_; }
		const std::vector<float>& Normals() const { return normals_; }
		const std::vector<float>& TexCoords() const { return texCoords_; }
		const std::vector<unsigned>& Indices() const { return indices_; }
		const std::vector<unsigned>& LineIndices() const { return lineIndices_; }

	private:

		// static functions
		static void computeFaceNormal(const float v1[3], const float v2[3], const float v3[3], float normal[3]);
		static void computeVertexNormal(const float v[3], float normal[3]);
		static float computeScaleForLength(const float v[3], float length);
		static void computeHalfVertex(const float v1[3], const float v2[3], float length, float newV[3]);
		static void computeHalfTexCoord(const float t1[2], const float t2[2], float newT[2]);
		static bool isSharedTexCoord(const float t[2]);
		static bool isOnLineSegment(const float a[2], const float b[2], const float c[2]);

		// member functions
		std::vector<float> computeIcosahedronVertices() const;
		void buildVerticesFlat();
		void buildVerticesSmooth();
		void subdivideVerticesFlat();
		void subdivideVerticesSmooth();
		void addVertex(float x, float y, float z);
		void addVertices(const float v1[3], const float v2[3], const float v3[3]);
		void addNormal(float nx, float ny, float nz);
		void addNormals(const float n1[3], const float n2[3], const float n3[3]);
		void addTexCoord(float s, float t);
		void addTexCoords(const float t1[2], const float t2[2], const float t3[2]);
		void addIndices(unsigned int i1, unsigned int i2, unsigned int i3);
		void addSubLineIndices(unsigned int i1, unsigned int i2, unsigned int i3,
			unsigned int i4, unsigned int i5, unsigned int i6);
		unsigned int addSubVertexAttribs(const float v[3], const float n[3], const float t[2]);

		const float radius_;
		const int subdivision_;
		const bool smooth_;

		std::vector<float> vertices_;
		std::vector<float> normals_;
		std::vector<float> texCoords_;
		std::vector<unsigned int> indices_;
		std::vector<unsigned int> lineIndices_;
		std::map<std::pair<float, float>, unsigned int> sharedIndices_;   // indices of shared vertices, key is tex coord (s,t)
	};

}
