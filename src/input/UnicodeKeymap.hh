#ifndef UNICODEKEYMAP_HH
#define UNICODEKEYMAP_HH

#include "openmsx.hh"
#include "string_view.hh"
#include <vector>
#include <utility>
#include <cassert>

namespace openmsx {

/** A position (row, column) in a keyboard matrix.
  */
class KeyMatrixPosition final {
	static constexpr unsigned INVALID = 0xFF;

public:
	/** Rows are in the range [0..NUM_ROWS). */
	static constexpr unsigned NUM_ROWS = 16;
	/** Columns are in the range [0..NUM_COLS). */
	static constexpr unsigned NUM_COLS = 8;
	/** Combined row and column values are in the range [0..NUM_ROWCOL). */
	static constexpr unsigned NUM_ROWCOL = 1 << 7;

	/** Creates an invalid key matrix position, which can be used when
	  * a key does not exist on a particular keyboard.
	  */
	KeyMatrixPosition()
		: rowCol(INVALID)
	{
	}

	/** Creates a key matrix position from a byte: the row is stored in
	  * the high nibble, the column is stored in the low nibble.
	  */
	KeyMatrixPosition(byte rowCol_)
		: KeyMatrixPosition(rowCol_ >> 4, rowCol_ & 0x0F)
	{
	}

	/** Creates a key matrix position with a given row and column.
	  */
	KeyMatrixPosition(unsigned row, unsigned col)
		: rowCol((row << 3) | col)
	{
		assert(row < NUM_ROWS);
		assert(col < NUM_COLS);
		assert(isValid());
	}

	/** Returns true iff this position is valid.
	  */
	bool isValid() const {
		return rowCol != INVALID;
	}

	/** Returns the matrix row.
	  * Must only be called on valid positions.
	  */
	unsigned getRow() const {
		assert(isValid());
		return rowCol >> 3;
	}

	/** Returns the matrix column.
	  * Must only be called on valid positions.
	  */
	unsigned getColumn() const {
		assert(isValid());
		return rowCol & 0x07;
	}

	/** Returns the matrix row and column combined in a single byte: the column
	  * is stored in the lower 3 bits, the row is stored in the higher bits.
	  * Must only be called on valid positions.
	  */
	byte getRowCol() const {
		assert(isValid());
		return rowCol;
	}

	/** Returns a mask with the bit corresponding to this position's
	  * column set, all other bits clear.
	  * Must only be called on valid positions.
	  */
	unsigned getMask() const {
		assert(isValid());
		return 1 << getColumn();
	}

	bool operator==(const KeyMatrixPosition& other) const {
		return rowCol == other.rowCol;
	}

private:
	byte rowCol;
};

class UnicodeKeymap
{
public:
	struct KeyInfo {
		enum Modifier { SHIFT, CTRL, GRAPH, CAPS, CODE, NUM_MODIFIERS };
		// Modifier masks:
		static constexpr byte SHIFT_MASK = 1 << SHIFT;
		static constexpr byte CTRL_MASK  = 1 << CTRL;
		static constexpr byte GRAPH_MASK = 1 << GRAPH;
		static constexpr byte CAPS_MASK  = 1 << CAPS;
		static constexpr byte CODE_MASK  = 1 << CODE;

		KeyInfo(KeyMatrixPosition pos_, byte modmask_)
			: pos(pos_), modmask(modmask_)
		{
			assert(pos.isValid());
		}
		KeyInfo()
			: pos(), modmask(0)
		{
		}
		bool isValid() const {
			return pos.isValid();
		}
		KeyMatrixPosition pos;
		byte modmask;
	};

	explicit UnicodeKeymap(string_view keyboardType);

	KeyInfo get(unsigned unicode) const;
	KeyInfo getDeadkey(unsigned n) const;

	/** Returns a mask in which a bit is set iff the corresponding modifier
	  * is relevant for the given key. A modifier is considered relevant if
	  * there is at least one mapping entry for the key that requires the
	  * modifier to be active.
	  * Must only be called on valid KeyInfos.
	  */
	byte getRelevantMods(const KeyInfo& keyInfo) const {
		return relevantMods[keyInfo.pos.getRowCol()];
	}

private:
	static const unsigned NUM_DEAD_KEYS = 3;

	void parseUnicodeKeymapfile(string_view data);

	std::vector<std::pair<unsigned, KeyInfo>> mapdata;
	/** Contains a mask for each key matrix position, which for each modifier
	  * has the corresponding bit set if that modifier that affects the key.
	  */
	byte relevantMods[KeyMatrixPosition::NUM_ROWCOL];
	KeyInfo deadKeys[NUM_DEAD_KEYS];
};

} // namespace openmsx

#endif
