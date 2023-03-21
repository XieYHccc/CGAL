#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/Sparse>

using namespace Ubpa;

using namespace std;
using namespace Eigen;

MinSurf::MinSurf(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void MinSurf::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool MinSurf::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	// init half-edge structure
	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	heMesh->Init(triangles);

	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is not a triangle mesh or hasn't a boundaries\n");
		heMesh->Clear();
		return false;
	}

	// triangle mesh's positions ->  half-edge structure's positions
	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}

	this->triMesh = triMesh;
	return true;
}

bool MinSurf::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	Minimize();

	// half-edge structure -> triangle mesh
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();
	vector<pointf3> positions;
	vector<unsigned> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);
	for (auto v : heMesh->Vertices())
		positions.push_back(v->pos.cast_to<pointf3>());
	for (auto f : heMesh->Polygons()) { // f is triangle
		for (auto v : f->BoundaryVertice()) // vertices of the triangle
			indice.push_back(static_cast<unsigned>(heMesh->Index(v)));
	}

	triMesh->Init(indice, positions);

	return true;
}

void MinSurf::Minimize() {
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();
	SparseMatrix<float>A(nV,nV);
	Eigen::MatrixXf b(nV, 3);
	b.fill(0.f);
	Eigen::MatrixXf X(nV, 3);
	vector<Triplet<float>> T_list;
	//SimplicialCholesky<SparseMatrix<float>> solver; 矩阵非正定不能能用cholesky方法分解
	SparseLU<SparseMatrix<float>> solver;

	//get A, bx, by, and bz
	for (auto v : heMesh->Vertices())
	{
		size_t index = heMesh->Index(v);

		if (!v->IsBoundary())
		{
			std::vector<V*> adjVs = v->AdjVertices();
			T_list.push_back(Eigen::Triplet<float>(index, index, (float)(adjVs.size())));
			for (auto neighbor : adjVs)
			{
				T_list.push_back(Triplet<float>(index, heMesh->Index(neighbor), -1.f));
			}
		}
		else
		{
			T_list.push_back(Triplet<float>(index, index, 1.f));
			b(index, 0) = v->pos[0];
			b(index, 1) = v->pos[1];
			b(index, 2) = v->pos[2];
		}
	}

	A.setFromTriplets(T_list.begin(), T_list.end());
	// compress the matrix to save memory
	cout << A << endl;
	A.makeCompressed();

	solver.compute(A);
	X = solver.solve(b);

	for (auto v : heMesh->Vertices())
	{
		size_t index = heMesh->Index(v);
		v->pos = Ubpa::vecf3(X(index,0),X(index,1),X(index,2));
	}
}
