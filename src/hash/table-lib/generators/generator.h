#ifndef GENERATOR_H
#define GENERATOR_H

namespace Hash { namespace Generators {

	template<typename T>
	class Generator {
	public:

		virtual ~Generator(void) {
		}
		
		virtual void setSeed(T aSeed) = 0;

		virtual void initialize(void) = 0;

		virtual void setFrom(T min) = 0;

		virtual void setPartLength(size_t) = 0;

		virtual void setTableLength(size_t) = 0;

		virtual T generate(void) = 0;

	};

} }

#endif /* GENERATOR_H */
