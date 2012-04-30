/**
 * @file   PCNErrorMap.hxx
 * @author Suvayu Ali <Suvayu.Ali@cern.ch>
 * @date   Sat Mar 24 22:20:12 2012
 *
 * @brief  Definition for the PCNError and PCNErrorMap classes.
 *
 *         As the name indicates, the PCNError class defines a PCN
 *         error in terms of the expected (correct) Pipeline Column
 *         Number and the exclusive OR (XOR) of the faulty PCN value
 *         with the correct PCN.
 *
 *         The PCNErrorMap class defines a map of the faulty bits for
 *         each Beetle chip reporting PCN errors. The maps are
 *         represented as 2-dimensional histograms.
 *
 */

#ifndef __PCNERRORMAP_HXX
#define __PCNERRORMAP_HXX


#include <string>
#include <bitset>
#include <map>

#include <TH2D.h>
// #include <TCanvas.h>


/// PCNError defines PCN errors from PCN and XOR bits
class PCNError {
public:

  /// Define different bit types.
  enum _PCN_T {
    kPCN,			/**< Correct PCN */
    kXOR,			/**< Exclusive OR of bad PCN with correct PCN */
    kBAD			/**< Incorrect PCN */
  };

  typedef std::bitset<8> eightbits; /**< Typedef for 8 bit bitset. */

  /**
   * Constructor to initialise from unsigned integers
   *
   * @param pcn Correct PCN
   * @param pcnxor Exclusive OR with correct PCN
   */
  PCNError(unsigned int pcn, unsigned int pcnxor);

  /**
   * Constructor to initialise from bit strings
   *
   * @param pcnbits Correct PCN bit string
   * @param xorbits Exclusive OR bit string
   */
  PCNError(std::string pcnbits, std::string xorbits);
  ~PCNError();

  /**
   * Get bitset for given bit type
   *
   * @param bittype Bit type to return
   *
   * @return Returned bitset
   */
  eightbits getBits(_PCN_T bittype=kPCN) const;

  /**
   * Set PCN bits from unsigned integers
   *
   * @param pcn Correct PCN
   * @param pcnxor Exclusive OR with correct PCN
   */
  void      setBits(unsigned int pcn, unsigned int pcnxor);

  /**
   * Set PCN bits from bit strings
   *
   * @param pcnbits Correct PCN bit string
   * @param xorbits Exclusive OR bit string
   */
  void      setBits(std::string pcnbits, std::string xorbits);

  friend class PCNErrorMap;	// probably unnecessary

private:

  unsigned int _pcn;		/**< Correct PCN */
  unsigned int _xor;		/**< Exclusive OR with correct PCN */
};


/// Key implements a mapping between TELL1 id and Beetle number to an integer
class Key {
public:

  /// Define bit lengths.
  enum _BITWORD {
    kBEETLE = 4,		/**< Beetle chip number (0-15). */
    kTELL1ID = 8		/**< TELL1 board number (0-131). */
  };

  /**
   * Constructor initialised with the TELL1 id and Beetle chip number.
   *
   * The two numbers are bitshifted and stored as a single number. The
   * key is initialised as: \f$ key = TELL1 \oplus (Beetle << 4) \f$.
   *
   * @param tell1id TELL1 board number
   * @param beetle Beetle chip number
   */
  Key(unsigned int tell1id, unsigned int beetle);
  ~Key();

  /** 
   * Return TELL1 id or Beetle number depending on type
   *
   * @param type _BITWORD type to return
   *
   * @return TELL1 id or Beetle number
   */
  unsigned int get(_BITWORD type) const;
  operator unsigned int () const; /**< Type conversion operator for unsigned integers. */

private:

  unsigned int _key;		/**< Beetle key. */
};


/// PCNErrorMap implements a map for PCN errors
class PCNErrorMap {
public:

  typedef std::map<unsigned int, TH2D*> TH2DMap; /**< Typedef for a histogram map. */
  typedef std::map<unsigned int, long>  longMap; /**< Typedef for a long map. */

  /**
   * Constructor initialised from total number of TELL1 boards.
   *
   * @param tell1s Total number of TELL1 boards
   */
  PCNErrorMap(unsigned int tell1s);
  ~PCNErrorMap();

  /**
   * Set debug mode. This changes message verbosity.
   *
   * @param debug Debug mode boolean
   */
  void setDebug(bool debug);

  /**
   * Fill the PCN error map.
   *
   * Fill the underlying histograms describing the PCN error map as
   * per TELL1 board ids, Beetle numbers and reported PCN error. The
   * bins are filled in reverse, MSB to LSB. This is done to
   * correspond with how we would write a binary bit on paper.
   *
   * @param tell1id TELL1 board id
   * @param beetle Beetle number
   * @param err PCN error
   */
  void Fill(unsigned int tell1id, unsigned int beetle, PCNError err);

  /**
   * Draw all underlying histograms on the same canvas.
   *
   * @param opts Drawing options (recommended options: COLZ)
   */
  void Draw(std::string opts);

private:

  // counters
  longMap ErrCounter;		/**< A sparse PCN error map. */
  bool    _debug;		/**< Debug option (changes verbosity). */

  // PCN error maps
  TH2D    hBeetleMap;		/**< PCN error map histogram for all Beetle chips. */
  TH2DMap hperBeetleBitMap;	/**< Map of faulty bits for all Beetles with PCN error. */
};


#endif	// __PCNERRORMAP_HXX
