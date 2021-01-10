#pragma once

#include <nodes/GraphModel>
#include <nodes/StyleCollection>


using ConnectionId = QtNodes::ConnectionId;
using ConnectionPolicy = QtNodes::ConnectionPolicy;
using NodeFlag = QtNodes::NodeFlag;
using NodeFlags = QtNodes::NodeFlags;
using NodeId = QtNodes::NodeId;
using PortIndex = QtNodes::PortIndex;
using PortRole = QtNodes::PortRole;
using PortType = QtNodes::PortType;
using StyleCollection = QtNodes::StyleCollection;
using QtNodes::InvalidNodeId;

namespace std
{
template<>
struct hash<std::tuple<NodeId, PortType, PortIndex>>
{
  using Key = std::tuple<NodeId, PortType, PortIndex>;

  inline
  std::size_t

  operator()(Key const& key) const
  {
    std::size_t h = 0;
    hash_combine(h,
                 std::get<0>(key),
                 std::get<1>(key),
                 std::get<2>(key));
    return h;
  }

};
}

struct NodeData
{
  QSize size;
  QPointF pos;
};


class CustomGraphModel : public QtNodes::GraphModel
{
public:

  CustomGraphModel()
    : _lastNodeId{0}
  {}


  std::unordered_set<NodeId>
  allNodeIds() const override
  {
    return _nodeIds;
  }

  std::unordered_set<std::pair<PortIndex, NodeId>>
  connectedNodes(NodeId    nodeId,
                 PortType  portType,
                 PortIndex portIndex) const override
  {
    Q_UNUSED(nodeId);
    Q_UNUSED(portType);
    Q_UNUSED(portIndex);

    // No connected nodes in the default implementation.
    return std::unordered_set<std::pair<PortIndex, NodeId>>();
  }

  void
  addNode(NodeId const nodeId = InvalidNodeId)
  {
    if (nodeId == InvalidNodeId)
    {
      // Create new node.

      _nodeIds.insert(_lastNodeId++);
    }
    else
    {
      _nodeIds.insert(nodeId);
    }
  }

  void
  addConnection(ConnectionId const connectionId) override
  {
    Q_UNUSED(connectionId);
  }

  NodeFlags
  nodeFlags(NodeId nodeId) const override
  {
    Q_UNUSED(nodeId);

    return NodeFlag::Resizable;
  }

  QVariant
  nodeData(NodeId nodeId, NodeRole role) const override
  {
    Q_UNUSED(nodeId);

    switch (role)
    {
      case NodeRole::Position:
        return _nodeData[nodeId].pos;
        break;

      case NodeRole::Size:
        return _nodeData[nodeId].size;
        break;

      case NodeRole::CaptionVisible:
        return true;
        break;

      case NodeRole::Caption:
        return QString("Node");
        break;

      case NodeRole::Style:
        {
          auto style = StyleCollection::nodeStyle();
          return style.toJson().toVariant();
        }
        break;

      case NodeRole::NumberOfInPorts:
        return 1u;
        break;

      case NodeRole::NumberOfOutPorts:
        return 1u;
        break;

      case NodeRole::Widget:
        return QVariant();
        break;
    }
  }

  bool
  setNodeData(NodeId nodeId, NodeRole role, QVariant value) override
  {
    Q_UNUSED(nodeId);
    Q_UNUSED(role);
    Q_UNUSED(value);

    bool result = false;

    switch (role)
    {
      case NodeRole::Position:
        _nodeData[nodeId].pos = value.value<QPointF>();
        result = true;
        break;

      case NodeRole::Size:
        _nodeData[nodeId].size = value.value<QSize>();
        result = true;
        break;

      case NodeRole::CaptionVisible:
        break;

      case NodeRole::Caption:
        break;

      case NodeRole::Style:
        break;

      case NodeRole::NumberOfInPorts:
        break;

      case NodeRole::NumberOfOutPorts:
        break;

      case NodeRole::Widget:
        break;
    }

    return result;
  }

  QVariant
  portData(NodeId    nodeId,
           PortType  portType,
           PortIndex portIndex,
           PortRole  role) const override
  {
    switch (role)
    {
      case PortRole::Data:
        return QVariant();
        break;

      case PortRole::DataType:
        return QVariant();
        break;

      case PortRole::ConnectionPolicy:
        return QVariant::fromValue(ConnectionPolicy::One);
        break;

      case PortRole::CaptionVisible:
        return true;
        break;

      case PortRole::Caption:
        if (portType == PortType::In)
          return QString::fromUtf8("Port In");
        else
          return QString::fromUtf8("Port Out");

        break;
    }

    return QVariant();
  }

  bool
  setPortData(NodeId    nodeId,
              PortType  portType,
              PortIndex portIndex,
              PortRole  role) const override
  {
    Q_UNUSED(nodeId);
    Q_UNUSED(portType);
    Q_UNUSED(portIndex);
    Q_UNUSED(role);

    return false;
  }

  bool
  deleteConnection(ConnectionId const connectionId) override
  {
    return false;
  }

  bool
  deleteNode(NodeId const nodeId) override
  {
    return false;
  }

private:

  std::unordered_set<NodeId> _nodeIds;

  std::unordered_map<std::tuple<NodeId, PortType, PortIndex>,
                     std::unordered_map<PortIndex, NodeId>>
  _connectivity;

  mutable std::unordered_map<NodeId, NodeData> _nodeData;


  unsigned int _lastNodeId;

};
