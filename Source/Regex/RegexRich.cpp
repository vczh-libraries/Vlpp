#include "RegexRich.h"

namespace vl
{
	namespace regex_internal
	{

/***********************************************************************
Data Structures for Backtracking
***********************************************************************/

		class StateSaver
		{
		public:
			enum StateStoreType
			{
				Positive,
				Negative,
				Other
			};

			const wchar_t*			reading;					// Current reading position
			State*					currentState;				// Current state
			vint					minTransition;				// The first transition to backtrack
			vint					captureCount;				// Available capture count			(the list size may larger than this)
			vint					stateSaverCount;			// Available saver count			(the list size may larger than this)
			vint					extensionSaverAvailable;	// Available extension saver count	(the list size may larger than this)
			vint					extensionSaverCount;		// Available extension saver count	(during executing)
			StateStoreType			storeType;					// Reason to keep this record

			bool operator==(const StateSaver& saver)const
			{
				return
					reading == saver.reading &&
					currentState == saver.currentState &&
					minTransition == saver.minTransition &&
					captureCount == saver.captureCount;
			}
		};

		class ExtensionSaver
		{
		public:
			vint					previous;					// Previous extension saver index
			vint					captureListIndex;			// Where to write the captured text
			Transition*				transition;					// The extension begin transition (Capture, Positive, Negative)
			const wchar_t*			reading;					// The reading position

			bool operator==(const ExtensionSaver& saver)const
			{
				return
					captureListIndex == saver.captureListIndex &&
					transition == saver.transition &&
					reading == saver.reading;
			}
		};
	}

	template<>
	struct POD<regex_internal::StateSaver>
	{
		static const bool Result = true;
	};

	template<>
	struct POD<regex_internal::ExtensionSaver>
	{
		static const bool Result = true;
	};

	namespace regex_internal
	{
		using namespace collections;

		void Push(List<ExtensionSaver>& elements, vint& available, vint& count, const ExtensionSaver& element)
		{
			if(elements.Count()==count)
			{
				elements.Add(element);
			}
			else
			{
				elements[count]=element;
			}
			ExtensionSaver& current=elements[count];
			current.previous=available;
			available=count++;
		}

		ExtensionSaver Pop(List<ExtensionSaver>& elements, vint& available, vint& count)
		{
			ExtensionSaver& current=elements[available];
			available=current.previous;
			return current;
		}

		template<typename T, typename K>
		void PushNonSaver(List<T, K>& elements, vint& count, const T& element)
		{
			if(elements.Count()==count)
			{
				elements.Add(element);
			}
			else
			{
				elements[count]=element;
			}
			count++;
		}

		template<typename T, typename K>
		T PopNonSaver(List<T, K>& elements, vint& count)
		{
			return elements[--count];
		}
	}

	namespace regex_internal
	{
/***********************************************************************
CaptureRecord
***********************************************************************/

		bool CaptureRecord::operator==(const CaptureRecord& record)const
		{
			return capture==record.capture && start==record.start && length==record.length;
		}

/***********************************************************************
RichInterpretor
***********************************************************************/

		RichInterpretor::RichInterpretor(Automaton::Ref _dfa)
			:dfa(_dfa)
		{
			datas=new UserData[dfa->states.Count()];

			for(vint i=0;i<dfa->states.Count();i++)
			{
				State* state=dfa->states[i].Obj();
				vint charEdges=0;
				vint nonCharEdges=0;
				bool mustSave=false;
				for(vint j=0;j<state->transitions.Count();j++)
				{
					if(state->transitions[j]->type==Transition::Chars)
					{
						charEdges++;
					}
					else
					{
						if(state->transitions[j]->type==Transition::Negative ||
						   state->transitions[j]->type==Transition::Positive)
						{
							mustSave=true;
						}
						nonCharEdges++;
					}
				}
				datas[i].NeedKeepState=mustSave || nonCharEdges>1 || (nonCharEdges!=0 && charEdges!=0);
				state->userData=&datas[i];
			}
		}

		RichInterpretor::~RichInterpretor()
		{
			delete[] datas;
		}

		bool RichInterpretor::MatchHead(const wchar_t* input, const wchar_t* start, RichResult& result)
		{
			List<StateSaver> stateSavers;
			List<ExtensionSaver> extensionSavers;

			StateSaver currentState;
			currentState.captureCount=0;
			currentState.currentState=dfa->startState;
			currentState.extensionSaverAvailable=-1;
			currentState.extensionSaverCount=0;
			currentState.minTransition=0;
			currentState.reading=input;
			currentState.stateSaverCount=0;
			currentState.storeType=StateSaver::Other;

			while (!currentState.currentState->finalState)
			{
				bool found = false; // true means at least one transition matches the input
				StateSaver oldState = currentState;
				// Iterate through all transitions from the current state
				for (vint i = currentState.minTransition; i < currentState.currentState->transitions.Count(); i++)
				{
					Transition* transition = currentState.currentState->transitions[i];
					switch (transition->type)
					{
					case Transition::Chars:
						{
							// match the input if the current character fall into the range
							CharRange range = transition->range;
							found =
								range.begin <= *currentState.reading &&
								range.end >= *currentState.reading;
							if (found)
							{
								currentState.reading++;
							}
						}
						break;
					case Transition::BeginString:
						{
							// match the input if this is the first character, and it is not consumed
							found = currentState.reading == start;
						}
						break;
					case Transition::EndString:
						{
							// match the input if this is after the last character, and it is not consumed
							found = *currentState.reading == L'\0';
						}
						break;
					case Transition::Nop:
						{
							// match without any condition
							found = true;
						}
						break;
					case Transition::Capture:
						{
							// Push the capture information
							ExtensionSaver saver;
							saver.captureListIndex = currentState.captureCount;
							saver.reading = currentState.reading;
							saver.transition = transition;
							Push(extensionSavers, currentState.extensionSaverAvailable, currentState.extensionSaverCount, saver);

							// Push the capture record, and it will be written if the input matches the regex
							CaptureRecord capture;
							capture.capture = transition->capture;
							capture.start = currentState.reading - start;
							capture.length = -1;
							PushNonSaver(result.captures, currentState.captureCount, capture);

							found = true;
						}
						break;
					case Transition::Match:
						{
							vint index = 0;
							for (vint j = 0; j < currentState.captureCount; j++)
							{
								CaptureRecord& capture = result.captures[j];
								// If the capture name matched
								if (capture.capture == transition->capture)
								{
									// If the capture index matched, or it is -1
									if (capture.length != -1 && (transition->index == -1 || transition->index == index))
									{
										// If the captured text matched
										if (wcsncmp(start + capture.start, currentState.reading, capture.length) == 0)
										{
											// Consume so much input
											currentState.reading += capture.length;
											found = true;
											break;
										}
									}

									// Fail if f the captured text with the specified name and index doesn't match
									if (transition->index != -1 && index == transition->index)
									{
										break;
									}
									else
									{
										index++;
									}
								}
							}
						}
						break;
					case Transition::Positive:
						{
							// Push the positive lookahead information
							ExtensionSaver saver;
							saver.captureListIndex = -1;
							saver.reading = currentState.reading;
							saver.transition = transition;
							Push(extensionSavers, currentState.extensionSaverAvailable, currentState.extensionSaverCount, saver);

							// Set found = true so that PushNonSaver(oldState) happens later
							oldState.storeType = StateSaver::Positive;
							found = true;
						}
						break;
					case Transition::Negative:
						{
							// Push the positive lookahead information

							ExtensionSaver saver;
							saver.captureListIndex = -1;
							saver.reading = currentState.reading;
							saver.transition = transition;
							Push(extensionSavers, currentState.extensionSaverAvailable, currentState.extensionSaverCount, saver);

							// Set found = true so that PushNonSaver(oldState) happens later
							oldState.storeType = StateSaver::Negative;
							found = true;
						}
						break;
					case Transition::NegativeFail:
						{
							// NegativeFail will be used when the nagative lookahead failed
						}
						break;
					case Transition::End:
						{
							// Find the corresponding extension saver so that we can know how to deal with a matched sub regex that ends here
							ExtensionSaver extensionSaver = Pop(extensionSavers, currentState.extensionSaverAvailable, currentState.extensionSaverCount);
							switch (extensionSaver.transition->type)
							{
							case Transition::Capture:
								{
									// Write the captured text
									CaptureRecord& capture = result.captures[extensionSaver.captureListIndex];
									capture.length = (currentState.reading - start) - capture.start;
									found = true;
								}
								break;
							case Transition::Positive:
								// Find the last positive lookahead state saver
								for (vint j = currentState.stateSaverCount - 1; j >= 0; j--)
								{
									StateSaver& stateSaver = stateSavers[j];
									if (stateSaver.storeType == StateSaver::Positive)
									{
										// restore the parsing state just before matching the positive lookahead, since positive lookahead doesn't consume input
										oldState.reading = stateSaver.reading;
										oldState.stateSaverCount = j;
										currentState.reading = stateSaver.reading;
										currentState.stateSaverCount = j;
										break;
									}
								}
								found = true;
								break;
							case Transition::Negative:
								// Find the last negative lookahead state saver
								for (vint j = currentState.stateSaverCount - 1; j >= 0; j--)
								{
									StateSaver& stateSaver = stateSavers[j];
									if (stateSaver.storeType == StateSaver::Negative)
									{
										// restore the parsing state just before matching the negative lookahead, since positive lookahead doesn't consume input
										oldState = stateSaver;
										oldState.storeType = StateSaver::Other;
										currentState = stateSaver;
										currentState.storeType = StateSaver::Other;
										i = currentState.minTransition - 1;
										break;
									}
								}
								break;
							default:;
							}
						}
						break;
					default:;
					}
					
					// Save the parsing state when necessary
					if (found)
					{
						UserData* data = (UserData*)currentState.currentState->userData;
						if (data->NeedKeepState)
						{
							oldState.minTransition = i + 1;
							PushNonSaver(stateSavers, currentState.stateSaverCount, oldState);
						}
						currentState.currentState = transition->target;
						currentState.minTransition = 0;
						break;
					}
				}

				// If no transition from the current state can be used
				if (!found)
				{
					// If there is a chance to do backtracking
					if (currentState.stateSaverCount)
					{
						currentState = PopNonSaver(stateSavers, currentState.stateSaverCount);
						// minTransition - 1 is always valid since the value is stored with adding 1
						// So minTransition - 1 record the transition, which is the reason the parsing state is saved
						if (currentState.currentState->transitions[currentState.minTransition - 1]->type == Transition::Negative)
						{
							// Find the next NegativeFail transition
							// Because when a negative lookahead regex failed to match, it is actually succeeded
							// Since a negative lookahead means we don't want to match this regex
							for (vint i = 0; i < currentState.currentState->transitions.Count(); i++)
							{
								Transition* transition = currentState.currentState->transitions[i];
								if (transition->type == Transition::NegativeFail)
								{
									// Restore the state to the target of NegativeFail to let the parsing continue
									currentState.currentState = transition->target;
									currentState.minTransition = 0;
									currentState.storeType = StateSaver::Other;
									break;
								}
							}
						}
					}
					else
					{
						break;
					}
				}
			}

			if (currentState.currentState->finalState)
			{
				// Keep available captures if succeeded
				result.start = input - start;
				result.length = (currentState.reading - start) - result.start;
				for (vint i = result.captures.Count() - 1; i >= currentState.captureCount; i--)
				{
					result.captures.RemoveAt(i);
				}
				return true;
			}
			else
			{
				// Clear captures if failed
				result.captures.Clear();
				return false;
			}
		}

		bool RichInterpretor::Match(const wchar_t* input, const wchar_t* start, RichResult& result)
		{
			const wchar_t* read=input;
			while(*read)
			{
				if(MatchHead(read, start, result))
				{
					return true;
				}
				read++;
			}
			return false;
		}

		const List<WString>& RichInterpretor::CaptureNames()
		{
			return dfa->captureNames;
		}
	}
}