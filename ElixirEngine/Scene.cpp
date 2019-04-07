#include "stdafx.h"
#include "Scene.h"

using namespace Elixir;

const Node Elixir::Scene::CreateNode(Transform transform)
{
	Node node;
	XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&transform.Position));
	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&transform.Rotation));
	XMMATRIX scale = XMMatrixScalingFromVector(XMLoadFloat3(&transform.Scale));
	XMMATRIX transformation = scale * rotation * translation;

	XMStoreFloat4x4(&node.localTransform, transformation);
	return node;
}

void Elixir::Scene::InsertTransform(Transform transform)
{
	position.push_back(transform.Position);
	rotation.push_back(transform.Rotation);
	scale.push_back(transform.Scale);
}

Scene::Scene() :
	rootNode(0u)
{
	auto node = CreateNode(DefaultTransform);
	node.parent = -1;
	node.worldTransform = node.localTransform;
	nodeList.push_back(node);
	InsertTransform(DefaultTransform);
}

NodeID Elixir::Scene::CreateNode(NodeID parent, Transform transform)
{
	XMMATRIX parentTransform = XMLoadFloat4x4(&nodeList[parent].worldTransform);
	NodeID nodeId = (NodeID)nodeList.size();
	auto node = CreateNode(transform);
	node.parent = parent;
	XMMATRIX worldTransform = parentTransform * XMLoadFloat4x4(&node.localTransform);
	XMStoreFloat4x4(&node.worldTransform, worldTransform);
	nodeList.push_back(node);
	nodeList[parent].children.push_back(nodeId);
	InsertTransform(transform);
	return nodeId;
}


void Elixir::Scene::SetTransform(NodeID nodeId, const Transform & transform)
{
	position[nodeId] = transform.Position;
	rotation[nodeId] = transform.Rotation;
	scale[nodeId] = transform.Scale;
}

Scene::~Scene()
{
}
