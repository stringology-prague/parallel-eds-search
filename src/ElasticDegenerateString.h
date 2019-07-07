#ifndef EDS_TOOLS_SRC_ELASTICDEGENERATESTRING_H_
#define EDS_TOOLS_SRC_ELASTICDEGENERATESTRING_H_

#include <iostream>
#include <memory>
#include <vector>

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>


class ElasticDegenerateString {
public:

    using SegmentId = size_t;
    using VariantId = size_t;
    using VariantRange = std::pair<VariantId,VariantId>;
    using DataRange = std::pair<const uint8_t*,const uint8_t*>;

    ElasticDegenerateString() = default;

    ElasticDegenerateString(const ElasticDegenerateString&) = default;

    ElasticDegenerateString(ElasticDegenerateString&&) = default;

    ElasticDegenerateString& operator=(const ElasticDegenerateString&) = default;

    ElasticDegenerateString& operator=(ElasticDegenerateString&&) = default;

    ~ElasticDegenerateString() = default;

    size_t Size() const;

    size_t Segments() const;

    const VariantRange GetSegment(SegmentId sid) const;

    size_t SegmentSize(SegmentId sid) const;

    const DataRange GetVariant(VariantId vid) const;

    size_t VariantSize(VariantId variant) const;

    template <class Archive>
    void load(Archive &ar, const unsigned int version);

    template <class Archive>
    void save(Archive &ar, const unsigned int version) const;

    static ElasticDegenerateString LoadFromString(std::istream &is);

    static void SaveToString(const ElasticDegenerateString& eds, std::ostream &os);

    BOOST_SERIALIZATION_SPLIT_MEMBER()

private:
    // All data stored in a single memory range
    std::vector<uint8_t> data_;
    using span_t = std::pair<size_t,size_t>;

    // Segment index into the variant index - each entry delimits variants belonging to a segment
//    using variant_span_t = std::pair<decltype(data_)::const_iterator,decltype(data_)::const_iterator>;
    std::vector<span_t> segment_index_;

    // Variant index into data - each entry delimits a substring representing a variant
//    using segment_index_span_t = std::pair<decltype(segment_index_)::const_iterator,decltype(segment_index_)::const_iterator>;
    std::vector<span_t> variant_index_;

    friend class boost::serialization::access;
    friend std::ostream & operator << (std::ostream & os, const ElasticDegenerateString & eds);
    friend std::istream & operator >> (std::istream & is, ElasticDegenerateString & eds);

};

#endif //EDS_TOOLS_SRC_ELASTICDEGENERATESTRING_H_
