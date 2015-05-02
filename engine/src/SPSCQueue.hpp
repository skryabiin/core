#ifndef CORE_SPSC_QUEUE_HPP
#define CORE_SPSC_QUEUE_HPP

#include <SDL.h>
namespace core {

	template <typename T>
	struct SPSCQueue {

		SPSCQueue()
		{
			Node* n = new Node{};
			n->next = 0;
			_tail = _head = _first = _tailCopy = n;
		}

		~SPSCQueue()
		{
			Node* n = _first;
			do
			{
				Node* next = n->next;
				delete n;
				n = next;
			} while (n);
		}


		void enqueue(T value)
		{
			Node* n = allocNode();
			n->next = 0;
			n->value = value;
			_store(&_head->next, n);
			_head = n;
		}

		// returns 'false' if queue is empty 
		bool dequeue(T& value)
		{
			if (consume(&_tail->next))
			{
				value = _tail->next->value;
				store_release(&_tail, _tail->next);
				return true;
			}
			else
			{
				return false;
			}
		}

		int queueDepth() {
			return SDL_AtomicGet(_depth);
		}

	private:

		T _consume(T const* ptr) {
			T val = *const_cast<T const volatile*>(ptr);
			SDL_AtomicDecRef(_depth);
			return val;
		}
		void _store(T const* ptr, T& val) {
			SDL_AtomicIncRef(_depth);
			*const_cast<T volatile*>(ptr) = val;
		}

		SDL_atomic_t _depth;

		struct Node {
			Node* next;
			T value;
		};

		Node* _tail;
		Node* _head;
		Node* _first;
		Node* _tailCopy;

		Node* _allocNode() {

			if (_first != tail_copy_)
			{
				Node* n = _first;
				_first = _first->next;
				return n;
			}
			_tailCopy = consume(&_tail);
			if (_first != _tailCopy)
			{
				Node* n = _first;
				_first = _first->next;
				return n;
			}
			Node* n = new Node{};
			return n;
		}
		
	};

} //end namespace core

#endif