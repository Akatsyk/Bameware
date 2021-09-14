#pragma once

#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )
#define PAD( size ) uint8_t MACRO_CONCAT( _pad, __COUNTER__ )[ size ];

namespace SDK
{
	class NetChannel
	{
	private:
		PAD(0x14);

	public:
		bool m_processing_messages;		// 0x0014
		bool m_should_delete;			// 0x0015

	private:
		PAD(0x2);

	public:
		int m_out_seq;					// 0x0018 last send outgoing sequence number
		int m_in_seq;					// 0x001C last received incoming sequnec number
		int m_out_seq_ack;				// 0x0020 last received acknowledge outgoing sequnce number
		int m_out_rel_state;			// 0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
		int m_in_rel_state;				// 0x0028 state of incoming reliable data
		int m_choked_packets;			// 0x002C number of choked packets

	private:
		PAD(0x414);					// 0x0030
	};
}
