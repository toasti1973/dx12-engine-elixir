#include "stdafx.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::Instance = nullptr;

ResourceManager::ResourceManager(ID3D12Device* device)
{
	this->device = device;
	Instance = this;
}

ResourceManager * ResourceManager::CreateInstance(ID3D12Device * device)
{
	ResourceManager* rm = nullptr;
	if (Instance == nullptr)
	{
		rm = new ResourceManager(device);
	}

	return rm;
}

ResourceManager * ResourceManager::GetInstance()
{
	return Instance;
}

void ResourceManager::LoadTexture(
	ID3D12CommandQueue * commandQueue, 
	DeferredRenderer * renderer, 
	HashID textureID, 
	std::wstring filepath, 
	TextureFileType texFileType, 
	bool isCubeMap, 
	TextureViewType viewType
)
{
	auto texture = new Texture(renderer, device);
	texture->CreateTexture(filepath, texFileType, commandQueue, isCubeMap);
	textures.insert(std::pair<HashID, Texture*>(textureID, texture));
}

void ResourceManager::LoadMaterial(ID3D12CommandQueue* commandQueue, DeferredRenderer* renderer, MaterialLoadData loadData)
{
	auto material = new Material(
		renderer, 
		{
			loadData.AlbedoFile,
			loadData.NormalFile,
			loadData.RoughnessFile,
			loadData.MetalnessFile
		}, 
		device, 
		commandQueue
	);

	materials.insert(std::pair<HashID, Material*>(loadData.MaterialID, material));
}

void ResourceManager::LoadMaterials(ID3D12CommandQueue * commandQueue, DeferredRenderer * renderer, std::vector<MaterialLoadData> materials)
{
	for (auto &loadData : materials)
	{
		LoadMaterial(commandQueue, renderer, loadData);
	}
}

void ResourceManager::LoadMeshes(ID3D12GraphicsCommandList* commandList, std::vector<std::string> meshList)
{
	for (auto& m : meshList)
	{
		LoadMesh(commandList, m);
	}
}

void ResourceManager::LoadMesh(ID3D12GraphicsCommandList * commandList, std::string filePath)
{
	auto filename = GetFileNameWithoutExtension(filePath);
	auto sid = StringID(filename.c_str()).GetHash();
	LoadMesh(commandList, sid, filePath);
}

void ResourceManager::LoadMesh(ID3D12GraphicsCommandList * commandList, HashID hashId, std::string filePath)
{
	auto mesh = new Mesh(filePath, device, commandList);
	meshes.insert(std::pair<HashID, Mesh*>(hashId, mesh));
}

Mesh * ResourceManager::GetMesh(HashID hashId)
{
	return meshes[hashId];
}

Material * ResourceManager::GetMaterial(HashID materialID)
{
	return materials[materialID];
}

Texture * ResourceManager::GetTexture(HashID textureID)
{
	return textures[textureID];
}


ResourceManager::~ResourceManager()
{
	for (auto& m : meshes)
	{
		delete m.second;
	}

	for (auto& m : materials)
	{
		delete m.second;
	}

	for (auto& m : textures)
	{
		delete m.second;
	}
}
