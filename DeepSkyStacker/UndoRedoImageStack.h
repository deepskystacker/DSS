#pragma once
/****************************************************************************
**
** Copyright (C) 2026 David C. Partridge
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
#include <cstdint>
#include <deque>
#include "DeepStack.h"

namespace DSS
{
	class UndoRedoImageStack final
	{
		using StackIterator = std::deque<DeepStack>::iterator;
	public:
		std::deque<DeepStack> theStack;
		std::size_t currentIndex;

	public:
		UndoRedoImageStack()
		{
		}

		~UndoRedoImageStack()
		{}

		UndoRedoImageStack(const UndoRedoImageStack&) = delete;
		UndoRedoImageStack(UndoRedoImageStack&&) = delete;
		UndoRedoImageStack& operator=(const UndoRedoImageStack&) = delete;

		inline bool empty() const
		{
			return theStack.empty();
		}


		inline std::size_t size() const 
		{
			return theStack.size();
		}

		inline std::size_t index() const
		{
			return currentIndex;
		}

		inline UndoRedoImageStack& clear()
		{
			theStack.clear();
			currentIndex = 0;
			return *this;
		}

		inline UndoRedoImageStack& reset()
		{
			if (theStack.size() >= 2)
			{
				//
				// Point to the second entry and erase all entries from there to the end of the stack.
				//
				StackIterator it{ theStack.begin() }; ++it;
				theStack.erase(it, theStack.end());
				// Reset the current index to point to the first entry.
				currentIndex = 0;
			}
			return *this;
		}

		inline UndoRedoImageStack& moveForward()
		{
			ZASSERTSTATE((currentIndex + 1) < theStack.size());
			++currentIndex;
			return *this;
		}

		inline UndoRedoImageStack& moveBackward()
		{
			ZASSERTSTATE(currentIndex > 0);
			--currentIndex;
			return *this;
		}

		inline bool backwardAvailable()
		{
			ZASSERTSTATE((currentIndex) < theStack.size());
			return (currentIndex >= 1);
		}

		inline bool forwardAvailable()
		{
			ZASSERTSTATE((currentIndex) < theStack.size());
			return (currentIndex + 1 < size());
		}

		inline DeepStack& at(std::size_t index)
		{
			return theStack.at(index);
		}

		inline DeepStack& current()
		{
			return at(currentIndex);
		}

		void add(const DeepStack& pp)
		{
			if ((theStack.size() > 1) && (currentIndex < theStack.size() - 1))
			{
				StackIterator it;
				size_t index = currentIndex + 1;

				for (it = theStack.begin(); it != theStack.end() && index > 0; it++, index--);

				theStack.erase(it, theStack.end());
			}

			theStack.emplace_back(pp);

			currentIndex = theStack.size() - 1;
		}

	};
}