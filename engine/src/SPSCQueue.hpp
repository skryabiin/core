#ifndef CORE_SPSC_QUEUE_HPP
#define CORE_SPSC_QUEUE_HPP

#include <SDL.h>
namespace core {

	template <typename T>
	struct SPSCQueue {

		SPSCQueue()
		{
			_setQueueDepth(0);
			clear();
		}

		~SPSCQueue()
		{
			_drain();
		}



		void clear() {
			if (queueDepth() > 0) {
				_drain();
			}

			Node* n = new Node{};
			n->next = 0;
			_tail = _head = _first = _tailCopy = n;
		}

		void enqueue(T value)
		{
			Node* n = _allocNode();
			n->next = 0;
			n->value = value;
			_store(&_head->next, n);
			_head = n;
			SDL_AtomicIncRef(&_depth);
		}

		// returns 'false' if queue is empty 
		bool dequeue(T& value)
		{
			if (_consume(&_tail->next))
			{
				value = _tail->next->value;
				_store(&_tail, _tail->next);
				SDL_AtomicDecRef(&_depth);
				return true;
			}
			else
			{
				return false;
			}
		}

		int queueDepth() {
			return SDL_AtomicGet(&_depth);
		}

		

	private:
		struct Node {
			Node* next;
			T value;
		};
		Node* _consume(Node** ptr) {
			Node* val = *ptr;

			return val;
		}
		void _store(Node** ptr, Node* val) {

			*ptr = val;
		}

		void _setQueueDepth(int depth) {
			SDL_AtomicSet(&_depth, depth);
		}

		void _drain() {
			Node* n = _first;
			do
			{
				Node* next = n->next;
				delete n;
				n = next;
			} while (n);
			_setQueueDepth(0);
		}
		
		SDL_atomic_t _depth;

		

		Node* _tail;
		Node* _head;
		Node* _first;
		Node* _tailCopy;

		Node* _allocNode() {

			if (_first != _tailCopy)
			{
				Node* n = _first;
				_first = _first->next;
				return n;
			}
			_tailCopy = _consume(&_tail);
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