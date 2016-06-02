/** ****************************************************************************
 * @file   HitCreator.h
 * @brief  Helper functions to create a hit
 * @date   December 18, 2014
 * @author petrillo@fnal.gov
 * @see    Hit.h HitCreator.cxx
 * 
 * ****************************************************************************/

#ifndef HITCREATOR_H
#define HITCREATOR_H

// C/C++ standard library
#include <string>
#include <vector>
#include <utility> // std::move()

// framework libraries
#include "canvas/Utilities/InputTag.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Provenance/ProductID.h"

// LArSoft libraries
#include "lardataobj/RawData/RawDigit.h"
#include "lardata/RecoBase/Wire.h"
#include "lardata/RecoBase/Hit.h"


// declaration of some heavy art stuff
namespace art {
	class Event;
	class EDProducer;
	class EDProductGetter;
} // namespace art


/// Reconstruction base classes
namespace recob {

  /** **************************************************************************
   * @brief Class managing the creation of a new recob::Hit object
   *
   * In order to be as simple as possible (Plain Old Data), data products like
   * recob::Hit need to be stripped of most of their functions, including the
   * ability to communicate whether a value we try to store is invalid
   * (that would require a art::Exception -- art -- or at least a message on the
   * screen -- MessageFacility) and the ability to read things from event,
   * services (e.g. geometry) etc.
   * 
   * A Creator is a class that creates a temporary data product, and at the
   * end it yields it to the caller for storage.
   * This last step should be by move construction, although a copy method is
   * also provided.
   * 
   * An example of creating a Hit object:
   * @todo Add the example!
   *     
   *     recob::HitCreator hit(...);
   *     hit.push_back(hit.move()); // hit content is not valid any more
   *     
   * This is a one-step creation object: the hit is constructed at the same
   * time the HitCreator is, and no facility is offered to modify the
   * constructed hit, or to create another one.
   * 
   * The constructors currently provided are:
   * 1. from RawDigit (extracts channel, view and signal type [CVS] thanks to
   *    geometry)
   * 2. from Wire, [CVS]
   * 3. from Wire, [CVS], summedADC is automatically computed from wire
   * 4. from Wire, [CVS], start and stop time from a region of interest
   * 5. from Wire, [CVS], start and stop time from index of region of interest
   */
  class HitCreator {
    public:
      /// Type of one region of interest
      using RegionOfInterest_t = recob::Wire::RegionsOfInterest_t::datarange_t;
      
      // destructor, copy and move constructor and assignment as default
      
      /**
       * @brief Constructor: extracts some information from raw digit
       * @param digits a pointer to a raw::RawDigit (for channel, view, signal type)
       * @param wireID ID of the wire the hit is on
       * @param start_tick first tick in the region the hit was extracted from
       * @param end_tick first tick after the region the hit was extracted from
       * @param rms RMS of the signal hit, in TDC time units
       * @param peak_time time at peak of the signal, in TDC time units
       * @param sigma_peak_time uncertainty on time at peak, in TDC time units
       * @param peak_amplitude amplitude of the signal at peak, in ADC units
       * @param sigma_peak_amplitude uncertainty on amplitude at peak
       * @param hit_integral total charge integrated under the hit signal
       * @param hit_sigma_integral uncertainty on the total hit charge
       * @param summedADC total ADC count in the region assigned to the hit
       * @param multiplicity number of hits in the region it was extracted from
       * @param local_index index of this hit in the region it was extracted from
       * @param goodness_of_fit quality parameter for the hit
       * @param dof degrees of freedom in the definition of the hit shape
       *
       * The information used from the raw digit is the channel ID; view and
       * signal type are obtained from geometry.
       */
      HitCreator(
        raw::RawDigit const& digits,
        geo::WireID const&   wireID,
        raw::TDCtick_t       start_tick,
        raw::TDCtick_t       end_tick,
        float                rms,
        float                peak_time,
        float                sigma_peak_time,
        float                peak_amplitude,
        float                sigma_peak_amplitude,
        float                hit_integral,
        float                hit_sigma_integral,
        float                summedADC,
        short int            multiplicity,
        short int            local_index,
        float                goodness_of_fit,
        int                  dof
        );
      
      
      /**
       * @brief Constructor: extracts some information from wire
       * @param wire a pointer to a recob::Wire (for channel, view, signal type)
       * @param wireID ID of the wire the hit is on
       * @param start_tick first tick in the region the hit was extracted from
       * @param end_tick first tick after the region the hit was extracted from
       * @param rms RMS of the signal hit, in TDC time units
       * @param peak_time time at peak of the signal, in TDC time units
       * @param sigma_peak_time uncertainty on time at peak, in TDC time units
       * @param peak_amplitude amplitude of the signal at peak, in ADC units
       * @param sigma_peak_amplitude uncertainty on amplitude at peak
       * @param hit_integral total charge integrated under the hit signal
       * @param hit_sigma_integral uncertainty on the total hit charge
       * @param summedADC total ADC count in the region assigned to the hit
       * @param multiplicity number of hits in the region it was extracted from
       * @param local_index index of this hit in the region it was extracted from
       * @param goodness_of_fit quality parameter for the hit
       * @param dof degrees of freedom in the definition of the hit shape
       *
       * The information used from the wire are the channel ID and view;
       * the signal type is obtained from geometry.
       */
      HitCreator(
        recob::Wire const&   wire,
        geo::WireID const&   wireID,
        raw::TDCtick_t       start_tick,
        raw::TDCtick_t       end_tick,
        float                rms,
        float                peak_time,
        float                sigma_peak_time,
        float                peak_amplitude,
        float                sigma_peak_amplitude,
        float                hit_integral,
        float                hit_sigma_integral,
        float                summedADC,
        short int            multiplicity,
        short int            local_index,
        float                goodness_of_fit,
        int                  dof
        );
      
      
      /**
       * @brief Constructor: computes sum of ADC from wire
       * @param wire a pointer to a recob::Wire (for channel, view, signal type)
       * @param wireID ID of the wire the hit is on
       * @param start_tick first tick in the region the hit was extracted from
       * @param end_tick first tick after the region the hit was extracted from
       * @param rms RMS of the signal hit, in TDC time units
       * @param peak_time time at peak of the signal, in TDC time units
       * @param sigma_peak_time uncertainty on time at peak, in TDC time units
       * @param peak_amplitude amplitude of the signal at peak, in ADC units
       * @param sigma_peak_amplitude uncertainty on amplitude at peak
       * @param hit_integral total charge integrated under the hit signal
       * @param hit_sigma_integral uncertainty on the total hit charge
       * @param multiplicity number of hits in the region it was extracted from
       * @param local_index index of this hit in the region it was extracted from
       * @param goodness_of_fit quality parameter for the hit
       * @param dof degrees of freedom in the definition of the hit shape
       *
       * The information used from the wire are the channel ID, view;
       * the signal type is obtained from geometry.
       * 
       * The sum of ADC counts is automatically computed over the whole range
       * of the wire signal between start_tick and end_tick
       * (the latter excluded).
       */
      HitCreator(
        recob::Wire const&   wire,
        geo::WireID const&   wireID,
        raw::TDCtick_t       start_tick,
        raw::TDCtick_t       end_tick,
        float                rms,
        float                peak_time,
        float                sigma_peak_time,
        float                peak_amplitude,
        float                sigma_peak_amplitude,
        float                hit_integral,
        float                hit_sigma_integral,
        short int            multiplicity,
        short int            local_index,
        float                goodness_of_fit,
        int                  dof
        );
      
      
      /**
       * @brief Constructor: uses region of interest specified by index
       * @param wire a pointer to a recob::Wire (for channel, view, signal type)
       * @param wireID ID of the wire the hit is on
       * @param rms RMS of the signal hit, in TDC time units
       * @param peak_time time at peak of the signal, in TDC time units
       * @param sigma_peak_time uncertainty on time at peak, in TDC time units
       * @param peak_amplitude amplitude of the signal at peak, in ADC units
       * @param sigma_peak_amplitude uncertainty on amplitude at peak
       * @param hit_integral total charge integrated under the hit signal
       * @param hit_sigma_integral uncertainty on the total hit charge
       * @param summedADC total ADC count in the region assigned to the hit
       * @param multiplicity number of hits in the region it was extracted from
       * @param local_index index of this hit in the region it was extracted from
       * @param goodness_of_fit quality parameter for the hit
       * @param dof degrees of freedom in the definition of the hit shape
       * @param SignalRoI the signal region the hit was extracted from
       *
       * The information used from the wire are the channel ID, view
       * and the region of interest; the signal type is obtained from
       * geometry.
       * 
       * Signal start and end ticks are extracted from the region of interest.
       */
      HitCreator(
        recob::Wire const&        wire,
        geo::WireID const&        wireID,
        float                     rms,
        float                     peak_time,
        float                     sigma_peak_time,
        float                     peak_amplitude,
        float                     sigma_peak_amplitude,
        float                     hit_integral,
        float                     hit_sigma_integral,
        float                     summedADC,
        short int                 multiplicity,
        short int                 local_index,
        float                     goodness_of_fit,
        int                       dof,
        RegionOfInterest_t const& signal
        );
      
      
      /**
       * @brief Constructor: uses region of interest specified by index
       * @param wire a pointer to a recob::Wire (for channel, view, signal type)
       * @param wireID ID of the wire the hit is on
       * @param rms RMS of the signal hit, in TDC time units
       * @param peak_time time at peak of the signal, in TDC time units
       * @param sigma_peak_time uncertainty on time at peak, in TDC time units
       * @param peak_amplitude amplitude of the signal at peak, in ADC units
       * @param sigma_peak_amplitude uncertainty on amplitude at peak
       * @param hit_integral total charge integrated under the hit signal
       * @param hit_sigma_integral uncertainty on the total hit charge
       * @param summedADC total ADC count in the region assigned to the hit
       * @param multiplicity number of hits in the region it was extracted from
       * @param local_index index of this hit in the region it was extracted from
       * @param goodness_of_fit quality parameter for the hit
       * @param dof degrees of freedom in the definition of the hit shape
       * @param iSignalRoI index in the wire of the signal region the hit was extracted from
       *
       * The information used from the wire are the channel ID, view
       * and the region of interest; the signal type is obtained from
       * geometry.
       * 
       * Signal start and end ticks are extracted from the region of interest.
       */
      HitCreator(
        recob::Wire const& wire,
        geo::WireID const& wireID,
        float              rms,
        float              peak_time,
        float              sigma_peak_time,
        float              peak_amplitude,
        float              sigma_peak_amplitude,
        float              hit_integral,
        float              hit_sigma_integral,
        float              summedADC,
        short int          multiplicity,
        short int          local_index,
        float              goodness_of_fit,
        int                dof,
        size_t             iSignalRoI
        );
      
      
      /**
       * @brief Constructor: copies from an existing hit
       * @param from the original hit
       */
      HitCreator(recob::Hit const& from);
      
      
      /**
       * @brief Constructor: copies from an existing hit, changing wire ID
       * @param from the original hit
       * @param wireID ID of the new wire the hit is on
       */
      HitCreator(recob::Hit const& from, geo::WireID const& wireID);
      
      
      /**
       * @brief Prepares the constructed hit to be moved away
       * @return a right-value reference to the constructed hit
       *
       * Despite the name, no move happens in this function.
       * Move takes place in the caller code as proper; for example:
       *     
       *     // be hit a HitCreator instance:
       *     std::vector<recob::Hit> Hits;
       *     hit.move();                        // nothing happens
       *     Hits.push_back(hit.move());        // here the copy happens
       *     recob::Hit single_hit(hit.move()); // wrong! hit is empty now
       *     
       */
      Hit&& move() { return std::move(hit); }
      
      
      /**
       * @brief Returns the constructed wire
       * @return a constant reference to the constructed wire
       *
       * Despite the name, no copy happens in this function.
       * Copy takes place in the caller code as proper; for example:
       *     
       *     // be Hit a HitCreator instance:
       *     std::vector<recob::Hit> Hits;
       *     hit.copy();                        // nothing happens
       *     Hits.push_back(hit.copy());        // here a copy happens
       *     recob::Hit single_hit(hit.copy()); // hit is copied again
       *     
       */
      const Hit& copy() const { return hit; }
      
    protected:
      
      Hit hit; ///< local instance of the hit being constructed
      
  }; // class HitCreator
  
  
  
  /** **************************************************************************
   * @brief Base class handling a collection of hits and its associations
   * 
   * Instead of creating a collection of hits, one for its association with
   * wires and one for its association with raw digits, one can use a class
   * derived from this one:
	* - HitCollectionCreator: push new hits one by one
	* - HitCollectionAssociator: push a complete collection of hits
	* - HitRefinerAssociator: push a complete collection of hits deriving their
	*     associations from other hits
   * Using put_into() will transfer into the event the data.
   * 
   * The typical usage is to have the constructor of the module call the static
   * function
   *     
   *     HitAndAssociationsWriterBase::declare_products(*this);
   *     
   * (this example declares a collection with empty instance name and that we
   * want associations to both wires and raw digits), and then in `produce()`:
   *     
   *     HitAndAssociationsWriterDerived hcol(*this, evt);
   *     
   *     // ... fill hcol in the proper way ...
   *     
   *     hcol.put_into(evt); // calls art::Event::put()
   *     
   */
  class HitAndAssociationsWriterBase {
      public:
    
    // no public constructor: use one of the derived classes!
    // destructor, copy and move constructors and assignment are default
    
    /// Returns the number of hits currently in the collection
    size_t size() const { return hits? hits->size(): 0; }
    
    
    /**
     * @brief Moves the data into an event
     * @param event the target event
     *
     * The calling module must have already declared the production of these
     * products with the proper instance name.
     * After the move, the collections in this object are empty.
     */
    void put_into(art::Event& event);
    
    
    /// Returns a read-only reference to the current list of hits
    std::vector<recob::Hit> const& peek() const { return *hits; }
    
    
    /**
     * @brief Declares the hit products we are going to fill
     * @param producer the module producing the data products
     * @param instance_name name of the instance for all data products
     * @param doWireAssns whether to enable associations to wires
     * @param doRawDigitAssns whether to enable associations to raw digits
     *
     * This declaration must be given in the constructor of producer.
     * It is equivalent to manually declare the relevant among these products:
     *     
     *     produces<std::vector<recob::Hit>>(prod_instance);
     *     produces<art::Assns<recob::Wire, recob::Hit>>(prod_instance);
     *     produces<art::Assns<raw::RawDigit, recob::Hit>>(prod_instance);
     *     
     * in the producer constructor.
     * All the data products (hit collection and associations) will have the
     * specified product instance name.
     */
    static void declare_products(
      art::EDProducer& producer, std::string instance_name = "",
      bool doWireAssns = true, bool doRawDigitAssns = true
      );
    
      protected:
    using HitPtr_t = art::Ptr<recob::Hit>; ///< type of art pointer to Hit
    
    std::string prod_instance; ///< name of the instance for data products
    
    std::unique_ptr<std::vector<recob::Hit>> hits; ///< collection of hits
    std::unique_ptr<art::Assns<recob::Wire, recob::Hit>> WireAssns; ///< associations with wires
    std::unique_ptr<art::Assns<raw::RawDigit, recob::Hit>> RawDigitAssns; ///< associations with raw digits
    
    
    art::ProductID hit_prodId; ///< stuff for creating art::Ptr
    art::EDProductGetter const* hit_getter; ///< stuff for creating art::Ptr
    
    
    //@{
    /**
     * @brief Constructor: sets instance name and whether to build associations
     * @param producer the module producing the data products
     * @param event the event the products are going to be put into
     * @param instance_name name of the instance for all data products
     * @param doWireAssns whether to enable associations to wires
     * @param doRawDigitAssns whether to enable associations to raw digits
     *
     * All the data products (hit collection and associations) will have the
     * specified product instance name.
     */
    HitAndAssociationsWriterBase(
      art::EDProducer& producer, art::Event& evt,
      std::string instance_name,
      bool doWireAssns, bool doRawDigitAssns
      );
    
    
    /// Creates an art pointer to the hit with the specified index
    HitPtr_t CreatePtr(size_t index) const;
    
  }; // class HitAndAssociationsWriterBase
  
  
  
  
  /** **************************************************************************
   * @brief A class handling a collection of hits and its associations
   * 
   * Instead of creating a collection of hits, one for its association with
   * wires and one for its association with raw digits, one can push hits into
   * this object, and then move it into the event.
   */
  class HitCollectionCreator: public HitAndAssociationsWriterBase {
      public:
    //@{
    /**
     * @brief Constructor: sets instance name and whether to build associations
     * @param producer the module producing the data products
     * @param event the event the products are going to be put into
     * @param instance_name name of the instance for all data products
     * @param doWireAssns whether to enable associations to wires
     * @param doRawDigitAssns whether to enable associations to raw digits
     *
     * All the data products (hit collection and associations) will have the
     * specified product instance name.
     */
    HitCollectionCreator(
      art::EDProducer& producer, art::Event& evt,
      std::string instance_name = "",
      bool doWireAssns = true, bool doRawDigitAssns = true
      );
    
    
    /**
     * @brief Constructor: no product instance name
     * @param producer the module producing the data products
     * @param event the event the products are going to be put into
     * @param doWireAssns whether to enable associations to wires
     * @param doRawDigitAssns whether to enable associations to raw digits
     */
    HitCollectionCreator(
      art::EDProducer& producer, art::Event& evt,
      bool doWireAssns, bool doRawDigitAssns
      ):
      HitCollectionCreator(producer, evt, "", doWireAssns, doRawDigitAssns)
      {}
    //@}
    
    // destructor, copy and move constructors and assignment are default
    
    
    //@{
    /**
     * @brief Adds the specified hit to the data collection
     * @param hit the hit that will be moved into the collection
     * @param wire art pointer to the wire to be associated to this hit
     * @param digits art pointer to the raw digits to be associated to this hit
     * 
     * After this call, hit will be invalid.
     * If a art pointer is not valid, that association will not be stored.
     */
    void emplace_back(
      recob::Hit&& hit,
      art::Ptr<recob::Wire> const& wire = art::Ptr<recob::Wire>(),
      art::Ptr<raw::RawDigit> const& digits = art::Ptr<raw::RawDigit>()
      );
    
    
    /**
     * @brief Adds the specified hit to the data collection
     * @param hit the hit that will be copied into the collection
     * @param wire art pointer to the wire to be associated to this hit
     * @param digits art pointer to the raw digits to be associated to this hit
     * 
     * If a art pointer is not valid, that association will not be stored.
     */
    void emplace_back(
      recob::Hit const& hit,
      art::Ptr<recob::Wire> const& wire = art::Ptr<recob::Wire>(),
      art::Ptr<raw::RawDigit> const& digits = art::Ptr<raw::RawDigit>()
      );
    
    
    /**
     * @brief Adds the specified hit to the data collection
     * @param hit the HitCreator object containing the hit
     * @param wire art pointer to the wire to be associated to this hit
     * @param digits art pointer to the raw digits to be associated to this hit
     *
     * After this call, the hit creator will be empty.
     * If a art pointer is not valid, that association will not be stored.
     */
    void emplace_back(
      HitCreator&& hit,
      art::Ptr<recob::Wire> const& wire = art::Ptr<recob::Wire>(),
      art::Ptr<raw::RawDigit> const& digits = art::Ptr<raw::RawDigit>()
      )
      { emplace_back(hit.move(), wire, digits); }
    
    
    /**
     * @brief Adds the specified hit to the data collection
     * @param hit the hit that will be moved into the collection
     * @param digits art pointer to the raw digits to be associated to this hit
     * 
     * After this call, hit will be invalid.
     * If the digit pointer is not valid, its association will not be stored.
     */
    void emplace_back(recob::Hit&& hit, art::Ptr<raw::RawDigit> const& digits)
      { emplace_back(std::move(hit), art::Ptr<recob::Wire>(), digits); }
    
    
    /**
     * @brief Adds the specified hit to the data collection
     * @param hit the HitCreator object containing the hit
     * @param digits art pointer to the raw digits to be associated to this hit
     * 
     * After this call, the hit creator will be empty.
     * If the digit pointer is not valid, its association will not be stored.
     */
    void emplace_back(HitCreator&& hit, art::Ptr<raw::RawDigit> const& digits)
      { emplace_back(std::move(hit), art::Ptr<recob::Wire>(), digits); }
    
    
    /**
     * @brief Adds the specified hit to the data collection
     * @param hit the HitCreator object containing the hit
     * @param digits art pointer to the raw digits to be associated to this hit
     * 
     * If the digit pointer is not valid, its association will not be stored.
     */
    void emplace_back
      (HitCreator const& hit, art::Ptr<raw::RawDigit> const& digits)
      { emplace_back(std::move(hit.copy()), art::Ptr<recob::Wire>(), digits); }
    //@}
    
    
    /// Returns the number of hits currently in the collection
    size_t size() const { return hits->size(); }
    
    
    /// Prepares the collection to host at least new_size hits
    void reserve(size_t new_size) { if (hits) hits->reserve(new_size); }
    
    
    /**
     * @brief Moves the data into an event
     * @param event the target event
     *
     * The calling module must have already declared the production of these
     * products with the proper instance name.
     * After the move, the collections in this object are empty.
     */
    void put_into(art::Event& event);
    
    
    /// Returns a read-only reference to the current list of hits
    std::vector<recob::Hit> const& peek() const { return *hits; }
    
    
      protected:
    using HitPtr_t = HitAndAssociationsWriterBase::HitPtr_t;
    
    /// Creates an art pointer to the hit with the last index
    HitPtr_t CreatePtrToLastHit() const
      { return hits->empty()? HitPtr_t(): CreatePtr(hits->size() - 1); }
    
    /// Creates associations bethween the last hit and the specified pointers
    void CreateAssociationsToLastHit(
      art::Ptr<recob::Wire> const& wire, art::Ptr<raw::RawDigit> const& digits
      );
    
  }; // class HitCollectionCreator
  
  
  
  
  /** **************************************************************************
   * @brief A class handling a collection of hits and its associations
   * 
   * Use this object if you already have a collection of recob::Hit and you
   * simply want the hits associated to the wire and digit with the same
   * channel.
   */
  class HitCollectionAssociator: public HitAndAssociationsWriterBase {
      public:
    //@{
    /**
     * @brief Constructor: sets instance name and whether to build associations
     * @param producer the module producing the data products
     * @param event the event the products are going to be put into
     * @param instance_name name of the instance for all data products
     * @param WireModuleLabel label of the module used to create wires
     * @param RawDigitModuleLabel label of the module used to create raw digits
     *
     * All the data products (hit collection and associations) will have the
     * specified product instance name.
     * 
     * If a label is empty, the corresponding association will not be produced.
     */
    HitCollectionAssociator(
      art::EDProducer& producer, art::Event& evt,
      std::string instance_name,
      art::InputTag const& WireModuleLabel,
      art::InputTag const& RawDigitModuleLabel
      );
    
    /**
     * @brief Constructor: sets instance name and whether to build associations
     * @param producer the module producing the data products
     * @param event the event the products are going to be put into
     * @param WireModuleLabel label of the module used to create wires
     * @param RawDigitModuleLabel label of the module used to create raw digits
     *
     * All the data products (hit collection and associations) will have a
     * default, empty product instance name.
     * 
     * If a label is empty, the corresponding association will not be produced.
     */
    HitCollectionAssociator(
      art::EDProducer& producer, art::Event& evt,
      art::InputTag const& WireModuleLabel,
      art::InputTag const& RawDigitModuleLabel
      ):
      HitCollectionAssociator
        (producer, evt, "", WireModuleLabel, RawDigitModuleLabel)
      {}
    
    /**
     * @brief Constructor: sets instance name and whether to build associations
     * @param producer the module producing the data products
     * @param event the event the products are going to be put into
     * @param instance_name name of the instance for all data products
     * @param WireModuleLabel label of the module used to create wires
     * @param doRawDigitAssns whether to write associations with raw digits
     *
     * All the data products (hit collection and associations) will have the
     * specified product instance name.
     * 
     * The raw digit association is built out of their existing associations
     * with wires, rather than by directly using the raw digits data product.
     */
    HitCollectionAssociator(
      art::EDProducer& producer, art::Event& evt,
      std::string instance_name,
      art::InputTag const& WireModuleLabel,
      bool doRawDigitAssns
      );
    //@}
    
    /**
     * @brief Constructor: sets instance name and whether to build associations
     * @param producer the module producing the data products
     * @param event the event the products are going to be put into
     * @param instance_name name of the instance for all data products
     * @param WireModuleLabel label of the module used to create wires
     * @param doRawDigitAssns whether to write associations with raw digits
     *
     * All the data products (hit collection and associations) will have the
     * default, empty product instance name.
     * 
     * The raw digit association is built out of their existing associations
     * with wires, rather than by directly using the raw digits data product.
     */
    HitCollectionAssociator(
      art::EDProducer& producer, art::Event& evt,
      art::InputTag const& WireModuleLabel,
      bool doRawDigitAssns
      ):
      HitCollectionAssociator
        (producer, evt, "", WireModuleLabel, doRawDigitAssns)
      {}

    //@}
    
    // destructor, copy and move constructors and assignment are default
    
    /**
     * @brief Uses the specified collection as data product
     * @param srchits the collection to be used as data product
     *
     * The very same collection is put into the event.
     * This object will temporary own the collection until the hits are put into
     * the event.
     * If there were previous hits in the object, they are lost.
     */
    void use_hits(std::unique_ptr<std::vector<recob::Hit>>&& srchits);
    
    
    /**
     * @brief Moves the data into an event
     * @param event the target event
     *
     * The calling module must have already declared the production of these
     * products with the proper instance name.
     * After the move, the collections in this object are empty.
     */
    void put_into(art::Event& event);
    
    
      protected:
    art::InputTag wires_label; ///< label of the collection of wires to associate
    art::InputTag digits_label; ///< label of raw digits collection to associate
    
    /// Finds out the associations for the current hits
    void prepare_associations
      (std::vector<recob::Hit> const& srchits, art::Event& event);
    void prepare_associations(art::Event& event)
      { prepare_associations(*hits, event); }
    
  }; // class HitCollectionAssociator
  
  /** **************************************************************************
   * @brief A class handling a collection of hits and its associations
   * 
   * Use this object if you already have a recob::Hit data product and
   * another collection that is going to become a data product, and you
   * simply want the new hits associated to the wire and digit with the same
   * channel.
   * No hit-to-hit association is attempted (that would be, incidentally, not
   * supported by art): the data product is used to get all the associated
   * wires and digits, then they are associated to the new hits by channel ID.
   * If a channel is not available, a warning is produced. If different hits
   * on the same channel are associated to different wires or raw digits, an
   * exception is thrown.
   */
  class HitRefinerAssociator: public HitAndAssociationsWriterBase {
      public:
    //@{
    /**
     * @brief Constructor: sets instance name and whether to build associations
     * @param producer the module producing the data products
     * @param event the event the products are going to be put into
     * @param HitModuleLabel label of the module used to create hits
     * @param instance_name name of the instance for all data products
     * @param doWireAssns whether to enable associations to wires
     * @param doRawDigitAssns whether to enable associations to raw digits
     *
     * All the data products (hit collection and associations) will have the
     * specified product instance name.
     */
    HitRefinerAssociator(
      art::EDProducer& producer, art::Event& evt,
      art::InputTag const& HitModuleLabel,
      std::string instance_name = "",
      bool doWireAssns = true, bool doRawDigitAssns = true
      );
    
    /**
     * @brief Constructor: sets instance name and whether to build associations
     * @param producer the module producing the data products
     * @param event the event the products are going to be put into
     * @param HitModuleLabel label of the module used to create hits
     * @param doWireAssns whether to enable associations to wires
     * @param doRawDigitAssns whether to enable associations to raw digits
     *
     * All the data products (hit collection and associations) will have an
     * empty product instance name.
     */
    HitRefinerAssociator(
      art::EDProducer& producer, art::Event& evt,
      art::InputTag const& HitModuleLabel,
      bool doWireAssns, bool doRawDigitAssns = true
      ):
      HitRefinerAssociator
        (producer, evt, HitModuleLabel, "", doWireAssns, doRawDigitAssns)
      {}
      
    //@}
    // destructor, copy and move constructors and assignment are default
    
    /**
     * @brief Uses the specified collection as data product
     * @param srchits the collection to be used as data product
     *
     * The very same collection is put into the event.
     * This object will temporary own the collection until the hits are put into
     * the event.
     * If there were previous hits in the object, they are lost.
     */
    void use_hits(std::unique_ptr<std::vector<recob::Hit>>&& srchits);
    
    
    /**
     * @brief Moves the data into an event
     * @param event the target event
     *
     * The calling module must have already declared the production of these
     * products with the proper instance name.
     * After the move, the collections in this object are empty.
     */
    void put_into(art::Event& event);
    
    
      protected:
    art::InputTag hits_label; ///< label of the collection of hits
    
    /// Finds out the associations for the current hits
    void prepare_associations
      (std::vector<recob::Hit> const& srchits, art::Event& event);
    void prepare_associations(art::Event& event)
      { prepare_associations(*hits, event); }
    
  }; // class HitRefinerAssociator
  
} // namespace recob

#endif // HITCREATOR_H