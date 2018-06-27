#include "ParsingTree.h"
#include "../Collections/Operation.h"
#include "../Collections/OperationForEach.h"

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif

namespace vl
{
	using namespace collections;

	namespace parsing
	{
		vint CompareTextRange(Ptr<ParsingTreeNode> r1, Ptr<ParsingTreeNode> r2)
		{
			return ParsingTextPos::Compare(r1->GetCodeRange().start, r2->GetCodeRange().start);
		}

/***********************************************************************
ParsingTreeNode::TraversalVisitor
***********************************************************************/

		ParsingTreeNode::TraversalVisitor::TraversalVisitor(TraverseDirection _direction)
			:direction(_direction)
		{
		}

		void ParsingTreeNode::TraversalVisitor::BeforeVisit(ParsingTreeToken* node)
		{
		}

		void ParsingTreeNode::TraversalVisitor::AfterVisit(ParsingTreeToken* node)
		{
		}

		void ParsingTreeNode::TraversalVisitor::BeforeVisit(ParsingTreeObject* node)
		{
		}

		void ParsingTreeNode::TraversalVisitor::AfterVisit(ParsingTreeObject* node)
		{
		}

		void ParsingTreeNode::TraversalVisitor::BeforeVisit(ParsingTreeArray* node)
		{
		}

		void ParsingTreeNode::TraversalVisitor::AfterVisit(ParsingTreeArray* node)
		{
		}

		void ParsingTreeNode::TraversalVisitor::Visit(ParsingTreeToken* node)
		{
			BeforeVisit(node);
			AfterVisit(node);
		}

		void ParsingTreeNode::TraversalVisitor::Visit(ParsingTreeObject* node)
		{
			BeforeVisit(node);
			switch(direction)
			{
			case TraverseDirection::ByTextPosition:
				{
					FOREACH(Ptr<ParsingTreeNode>, node, node->GetSubNodes())
					{
						node->Accept(this);
					}
				}
				break;
			case TraverseDirection::ByStorePosition:
				{
					FOREACH(Ptr<ParsingTreeNode>, node, node->GetMembers().Values())
					{
						node->Accept(this);
					}
				}
				break;
			}
			AfterVisit(node);
		}

		void ParsingTreeNode::TraversalVisitor::Visit(ParsingTreeArray* node)
		{
			BeforeVisit(node);
			switch(direction)
			{
			case TraverseDirection::ByTextPosition:
				{
					FOREACH(Ptr<ParsingTreeNode>, node, node->GetSubNodes())
					{
						node->Accept(this);
					}
				}
				break;
			case TraverseDirection::ByStorePosition:
				{
					FOREACH(Ptr<ParsingTreeNode>, node, node->GetItems())
					{
						node->Accept(this);
					}
				}
				break;
			}
			AfterVisit(node);
		}

/***********************************************************************
ParsingTreeNode
***********************************************************************/

		bool ParsingTreeNode::BeforeAddChild(Ptr<ParsingTreeNode> node)
		{
			return node->parent==0;
		}

		void ParsingTreeNode::AfterAddChild(Ptr<ParsingTreeNode> node)
		{
			node->parent=this;
			ClearQueryCache();
		}

		bool ParsingTreeNode::BeforeRemoveChild(Ptr<ParsingTreeNode> node)
		{
			return node->parent!=0;
		}

		void ParsingTreeNode::AfterRemoveChild(Ptr<ParsingTreeNode> node)
		{
			node->parent=0;
			ClearQueryCache();
		}

		ParsingTreeNode::ParsingTreeNode(const ParsingTextRange& _codeRange)
			:codeRange(_codeRange)
			,parent(0)
		{
		}

		ParsingTreeNode::~ParsingTreeNode()
		{
		}

		ParsingTextRange ParsingTreeNode::GetCodeRange()
		{
			return codeRange;
		}

		void ParsingTreeNode::SetCodeRange(const ParsingTextRange& range)
		{
			codeRange=range;
		}

		void ParsingTreeNode::InitializeQueryCache()
		{
			const NodeList& subNodes=GetSubNodesInternal();
			ClearQueryCache();
			auto subNodesExists = &subNodes;
			if(subNodesExists)
			{
				FOREACH(Ptr<ParsingTreeNode>, node, subNodes)
				{
					node->InitializeQueryCache();
				}

				//if (codeRange.start.IsInvalid() || codeRange.start.IsInvalid())
				{
					FOREACH(Ptr<ParsingTreeNode>, subNode, subNodes)
					{
						const auto& subRange = subNode->codeRange;
						const auto& min = !subRange.start.IsInvalid() ? subRange.start : subRange.end;
						const auto& max = !subRange.end.IsInvalid() ? subRange.end : subRange.start;

						if (codeRange.start.IsInvalid() || (!min.IsInvalid() && codeRange.start > min))
						{
							codeRange.start = min;
						}
						if (codeRange.end.IsInvalid() || (!max.IsInvalid() && codeRange.end < max))
						{
							codeRange.end = max;
						}
					}
				}

				CopyFrom(
					cachedOrderedSubNodes,
					From(subNodes)
						.Where([=](Ptr<ParsingTreeNode> node)
						{
							const auto& range = node->GetCodeRange();
							return !range.start.IsInvalid() && !range.end.IsInvalid();
						})
						.OrderBy(&CompareTextRange)
					);
			}
		}

		void ParsingTreeNode::ClearQueryCache()
		{
			cachedOrderedSubNodes.Clear();
		}

		ParsingTreeNode* ParsingTreeNode::GetParent()
		{
			return parent;
		}

		const ParsingTreeNode::NodeList& ParsingTreeNode::GetSubNodes()
		{
			return cachedOrderedSubNodes;
		}

		ParsingTreeNode* ParsingTreeNode::FindSubNode(const ParsingTextPos& position)
		{
			return FindSubNode(ParsingTextRange(position, position));
		}

		ParsingTreeNode* ParsingTreeNode::FindSubNode(const ParsingTextRange& range)
		{
			if (codeRange.start <= range.start && range.end <= codeRange.end)
			{
				vint start = 0;
				vint end = cachedOrderedSubNodes.Count() - 1;
				while (start <= end)
				{
					vint selected = (start + end) / 2;
					ParsingTreeNode* selectedNode = cachedOrderedSubNodes[selected].Obj();
					const ParsingTextRange& selectedRange = selectedNode->codeRange;
					if (range.end<selectedRange.start)
					{
						end = selected - 1;
					}
					else if (range.start>selectedRange.end)
					{
						start = selected + 1;
					}
					else if (selectedRange.start <= range.start && range.end <= selectedRange.end)
					{
						return selectedNode;
					}
					else
					{
						return this;
					}
				}
			}
			return this;
		}

		ParsingTreeNode* ParsingTreeNode::FindDeepestNode(const ParsingTextPos& position)
		{
			return FindDeepestNode(ParsingTextRange(position, position));
		}

		ParsingTreeNode* ParsingTreeNode::FindDeepestNode(const ParsingTextRange& range)
		{
			ParsingTreeNode* result=0;
			ParsingTreeNode* node=this;
			do
			{
				result=node;
				node=node->FindSubNode(range);
			}while(result!=node);
			return result;
		}

/***********************************************************************
ParsingTreeToken
***********************************************************************/

		const ParsingTreeToken::NodeList& ParsingTreeToken::GetSubNodesInternal()
		{
			return *(NodeList*)0;
		}

		ParsingTreeToken::ParsingTreeToken(const WString& _value, vint _tokenIndex, const ParsingTextRange& _codeRange)
			:ParsingTreeNode(_codeRange)
			,value(_value)
			,tokenIndex(_tokenIndex)
		{
		}

		ParsingTreeToken::~ParsingTreeToken()
		{
		}

		void ParsingTreeToken::Accept(IVisitor* visitor)
		{
			visitor->Visit(this);
		}

		Ptr<ParsingTreeNode> ParsingTreeToken::Clone()
		{
			Ptr<ParsingTreeToken> clone=new ParsingTreeToken(value, tokenIndex, codeRange);
			return clone;
		}

		vint ParsingTreeToken::GetTokenIndex()
		{
			return tokenIndex;
		}

		void ParsingTreeToken::SetTokenIndex(vint _tokenIndex)
		{
			tokenIndex=_tokenIndex;
		}

		const WString& ParsingTreeToken::GetValue()
		{
			return value;
		}

		void ParsingTreeToken::SetValue(const WString& _value)
		{
			value=_value;
		}

/***********************************************************************
ParsingTreeObject
***********************************************************************/

		const ParsingTreeObject::NodeList& ParsingTreeObject::GetSubNodesInternal()
		{
			return members.Values();
		}

		ParsingTreeObject::ParsingTreeObject(const WString& _type, const ParsingTextRange& _codeRange)
			:ParsingTreeNode(_codeRange)
			,type(_type)
		{
		}

		ParsingTreeObject::~ParsingTreeObject()
		{
		}

		void ParsingTreeObject::Accept(IVisitor* visitor)
		{
			visitor->Visit(this);
		}

		Ptr<ParsingTreeNode> ParsingTreeObject::Clone()
		{
			Ptr<ParsingTreeObject> clone=new ParsingTreeObject(type, codeRange);
			CopyFrom(clone->rules, rules);
			for(vint i=0;i<members.Count();i++)
			{
				WString name=members.Keys().Get(i);
				Ptr<ParsingTreeNode> node=members.Values().Get(i)->Clone();
				clone->SetMember(name, node);
			}
			return clone;
		}

		const WString& ParsingTreeObject::GetType()
		{
			return type;
		}

		void ParsingTreeObject::SetType(const WString& _type)
		{
			type=_type;
		}

		ParsingTreeObject::NodeMap& ParsingTreeObject::GetMembers()
		{
			return members;
		}

		Ptr<ParsingTreeNode> ParsingTreeObject::GetMember(const WString& name)
		{
			vint index=members.Keys().IndexOf(name);
			if(index==-1) return 0;
			return members.Values().Get(index);
		}

		bool ParsingTreeObject::SetMember(const WString& name, Ptr<ParsingTreeNode> node)
		{
			vint index=members.Keys().IndexOf(name);
			if(index!=-1)
			{
				Ptr<ParsingTreeNode> previous=members.Values().Get(index);
				if(previous==node) return true;
				if(!BeforeRemoveChild(previous) || !BeforeAddChild(node)) return false;
				members.Remove(name);
				AfterRemoveChild(previous);
			}
			members.Add(name, node);
			AfterAddChild(node);
			return true;
		}

		bool ParsingTreeObject::RemoveMember(const WString& name)
		{
			vint index=members.Keys().IndexOf(name);
			if(index!=-1)
			{
				Ptr<ParsingTreeNode> previous=members.Values().Get(index);
				if(BeforeRemoveChild(previous))
				{
					members.Remove(name);
					AfterRemoveChild(previous);
					return true;
				}
			}
			return false;
		}

		const ParsingTreeObject::NameList& ParsingTreeObject::GetMemberNames()
		{
			return members.Keys();
		}

		ParsingTreeObject::RuleList& ParsingTreeObject::GetCreatorRules()
		{
			return rules;
		}

/***********************************************************************
ParsingTreeArray
***********************************************************************/

		const ParsingTreeArray::NodeList& ParsingTreeArray::GetSubNodesInternal()
		{
			return items;
		}

		ParsingTreeArray::ParsingTreeArray(const WString& _elementType, const ParsingTextRange& _codeRange)
			:ParsingTreeNode(_codeRange)
			,elementType(_elementType)
		{
		}

		ParsingTreeArray::~ParsingTreeArray()
		{
		}

		void ParsingTreeArray::Accept(IVisitor* visitor)
		{
			visitor->Visit(this);
		}

		Ptr<ParsingTreeNode> ParsingTreeArray::Clone()
		{
			Ptr<ParsingTreeArray> clone=new ParsingTreeArray(elementType, codeRange);
			for(vint i=0;i<items.Count();i++)
			{
				Ptr<ParsingTreeNode> node=items.Get(i)->Clone();
				clone->AddItem(node);
			}
			return clone;
		}

		const WString& ParsingTreeArray::GetElementType()
		{
			return elementType;
		}

		void ParsingTreeArray::SetElementType(const WString& _elementType)
		{
			elementType=_elementType;
		}

		ParsingTreeArray::NodeArray& ParsingTreeArray::GetItems()
		{
			return items;
		}

		Ptr<ParsingTreeNode> ParsingTreeArray::GetItem(vint index)
		{
			if(0<=index && index<items.Count())
			{
				return items[index];
			}
			else
			{
				return 0;
			}
		}

		bool ParsingTreeArray::SetItem(vint index, Ptr<ParsingTreeNode> node)
		{
			if(0<=index && index<items.Count())
			{
				items[index]=node;
				return true;
			}
			else
			{
				return false;
			}
		}

		bool ParsingTreeArray::AddItem(Ptr<ParsingTreeNode> node)
		{
			return InsertItem(items.Count(), node);
		}

		bool ParsingTreeArray::InsertItem(vint index, Ptr<ParsingTreeNode> node)
		{
			if(0<=index && index<=items.Count())
			{
				if(BeforeAddChild(node))
				{
					items.Insert(index, node);
					AfterAddChild(node);
					return true;
				}
			}
			return false;
		}

		bool ParsingTreeArray::RemoveItem(vint index)
		{
			if(0<=index && index<items.Count())
			{
				Ptr<ParsingTreeNode> previous=items[index];
				if(BeforeRemoveChild(previous))
				{
					items.RemoveAt(index);
					AfterRemoveChild(previous);
					return true;
				}
			}
			return false;
		}

		bool ParsingTreeArray::RemoveItem(ParsingTreeNode* node)
		{
			return RemoveItem(items.IndexOf(node));
		}

		vint ParsingTreeArray::IndexOfItem(ParsingTreeNode* node)
		{
			return items.IndexOf(node);
		}

		bool ParsingTreeArray::ContainsItem(ParsingTreeNode* node)
		{
			return items.Contains(node);
		}

		vint ParsingTreeArray::Count()
		{
			return items.Count();
		}

		bool ParsingTreeArray::Clear()
		{
			FOREACH(Ptr<ParsingTreeNode>, node, items)
			{
				if(!BeforeRemoveChild(node)) return false;
			}
			FOREACH(Ptr<ParsingTreeNode>, node, items)
			{
				AfterRemoveChild(node);
			}
			items.Clear();
			return true;
		}

/***********************************************************************
ParsingError
***********************************************************************/

		ParsingError::ParsingError()
			:token(0)
			,parsingTree(0)
		{
		}

		ParsingError::ParsingError(const WString& _errorMessage)
			:token(0)
			,parsingTree(0)
			,errorMessage(_errorMessage)
		{
		}

		ParsingError::ParsingError(const regex::RegexToken* _token, const WString& _errorMessage)
			:token(_token)
			,parsingTree(0)
			,errorMessage(_errorMessage)
		{
			if(token)
			{
				codeRange.start.row=_token->rowStart;
				codeRange.start.column=_token->columnStart;
				codeRange.start.index=_token->start;
				codeRange.end.row=_token->rowEnd;
				codeRange.end.column=_token->columnEnd;
				codeRange.end.index=_token->start+_token->length-1;
				codeRange.codeIndex = _token->codeIndex;
			}
		}

		ParsingError::ParsingError(ParsingTreeCustomBase* _parsingTree, const WString& _errorMessage)
			:codeRange(_parsingTree->codeRange)
			,token(0)
			,parsingTree(_parsingTree)
			,errorMessage(_errorMessage)
		{
		}

		ParsingError::~ParsingError()
		{
		}

/***********************************************************************
ParsingEmptyPrintNodeRecorder
***********************************************************************/

		ParsingEmptyPrintNodeRecorder::ParsingEmptyPrintNodeRecorder()
		{
		}

		ParsingEmptyPrintNodeRecorder::~ParsingEmptyPrintNodeRecorder()
		{
		}

		void ParsingEmptyPrintNodeRecorder::Record(ParsingTreeCustomBase* node, const ParsingTextRange& range)
		{
		}

/***********************************************************************
ParsingMultiplePrintNodeRecorder
***********************************************************************/

		ParsingMultiplePrintNodeRecorder::ParsingMultiplePrintNodeRecorder()
		{
		}

		ParsingMultiplePrintNodeRecorder::~ParsingMultiplePrintNodeRecorder()
		{
		}

		void ParsingMultiplePrintNodeRecorder::AddRecorder(Ptr<IParsingPrintNodeRecorder> recorder)
		{
			recorders.Add(recorder);
		}

		void ParsingMultiplePrintNodeRecorder::Record(ParsingTreeCustomBase* node, const ParsingTextRange& range)
		{
			FOREACH(Ptr<IParsingPrintNodeRecorder>, recorder, recorders)
			{
				recorder->Record(node, range);
			}
		}

/***********************************************************************
ParsingOriginalLocationRecorder
***********************************************************************/

		ParsingOriginalLocationRecorder::ParsingOriginalLocationRecorder(Ptr<IParsingPrintNodeRecorder> _recorder)
			:recorder(_recorder)
		{
		}

		ParsingOriginalLocationRecorder::~ParsingOriginalLocationRecorder()
		{
		}

		void ParsingOriginalLocationRecorder::Record(ParsingTreeCustomBase* node, const ParsingTextRange& range)
		{
			auto codeRange = node->codeRange;
			codeRange.codeIndex = range.codeIndex;
			recorder->Record(node, codeRange);
		}

/***********************************************************************
ParsingGeneratedLocationRecorder
***********************************************************************/

		ParsingGeneratedLocationRecorder::ParsingGeneratedLocationRecorder(RangeMap& _rangeMap)
			:rangeMap(_rangeMap)
		{
		}

		ParsingGeneratedLocationRecorder::~ParsingGeneratedLocationRecorder()
		{
		}

		void ParsingGeneratedLocationRecorder::Record(ParsingTreeCustomBase* node, const ParsingTextRange& range)
		{
			rangeMap.Add(node, range);
		}

/***********************************************************************
ParsingUpdateLocationRecorder
***********************************************************************/

		ParsingUpdateLocationRecorder::ParsingUpdateLocationRecorder()
		{
		}

		ParsingUpdateLocationRecorder::~ParsingUpdateLocationRecorder()
		{
		}

		void ParsingUpdateLocationRecorder::Record(ParsingTreeCustomBase* node, const ParsingTextRange& range)
		{
			node->codeRange = range;
		}

/***********************************************************************
ParsingWriter
***********************************************************************/

		void ParsingWriter::HandleChar(wchar_t c)
		{
			lastPos = currentPos;
			switch (c)
			{
			case L'\n':
				currentPos.index++;
				currentPos.row++;
				currentPos.column = 0;
				break;
			default:
				currentPos.index++;
				currentPos.column++;
			}
		}

		ParsingWriter::ParsingWriter(stream::TextWriter& _writer, Ptr<IParsingPrintNodeRecorder> _recorder, vint _codeIndex)
			:writer(_writer)
			, recorder(_recorder)
			, codeIndex(_codeIndex)
			, lastPos(-1, 0, -1)
			, currentPos(0, 0, 0)
		{
		}

		ParsingWriter::~ParsingWriter()
		{
		}

		void ParsingWriter::WriteChar(wchar_t c)
		{
			writer.WriteChar(c);
			if (!recorder) return;
			HandleChar(c);
		}

		void ParsingWriter::WriteString(const wchar_t* string, vint charCount)
		{
			writer.WriteString(string, charCount);
			if (!recorder) return;
			for (vint i = 0; i < charCount; i++)
			{
				HandleChar(string[i]);
			}
		}

		void ParsingWriter::BeforePrint(ParsingTreeCustomBase* node)
		{
			if (!recorder) return;
			nodePositions.Add(NodePosPair(node, currentPos));
		}

		void ParsingWriter::AfterPrint(ParsingTreeCustomBase* node)
		{
			if (!recorder) return;

			auto pair = nodePositions[nodePositions.Count() - 1];
			nodePositions.RemoveAt(nodePositions.Count() - 1);
			CHECK_ERROR(pair.key == node, L"vl::parsing::ParsingWriter::AfterPrint(ParsingTreeNode*)#BeforePrint and AfterPrint should be call in pairs.");

			ParsingTextRange range(pair.value, lastPos, codeIndex);
			recorder->Record(node, range);
		}
	}
}

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic pop
#endif
