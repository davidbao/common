#ifndef ESCAPEOPTION_H
#define ESCAPEOPTION_H

namespace Net
{
	struct EscapeOption
	{
		uint8_t toEscapeBuffer[8];
		int toEscapeLength;
		uint8_t escapeBuffer[8];
		size_t escapeLength;
		off_t offset;
        size_t length;

	public:
		EscapeOption()
		{
			memset(toEscapeBuffer, 0, sizeof(toEscapeBuffer));
			toEscapeLength = 0;
			memset(escapeBuffer, 0, sizeof(escapeBuffer));
			escapeLength = 0;
			offset = 0;
			length = 0;
		}
	};
}

#endif // ESCAPEOPTION_H
