/**
 * @file get_network.cpp
 * @brief Getting network data from single-threaded CPU backend.
 * @author An. Vartenkov.
 * @date 21.05.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/backends/cpu-single-threaded/backend.h>
#include <knp/meta/variant_helpers.h>


namespace knp::backends::single_threaded_cpu
{
class PopulationValueIterator : public SingleThreadedCPUBackend::BaseValueIterator<core::AllPopulationsVariant>
{
public:
    PopulationValueIterator() = default;
    explicit PopulationValueIterator(const SingleThreadedCPUBackend::PopulationContainer::const_iterator &iter)
        : iter_(iter)
    {
    }
    bool operator==(const BaseValueIterator<core::AllPopulationsVariant> &rhs) const override
    {
        if (typeid(*this) != typeid(rhs)) return false;
        return dynamic_cast<const PopulationValueIterator &>(rhs).iter_ == iter_;
    }

    BaseValueIterator<core::AllPopulationsVariant> &operator++() override
    {
        ++iter_;
        return *this;
    }
    core::AllPopulationsVariant operator*() const override { return knp::meta::variant_cast(*iter_); }

private:
    SingleThreadedCPUBackend::PopulationContainer::const_iterator iter_;
};


class ProjectionValueIterator : public SingleThreadedCPUBackend::BaseValueIterator<core::AllProjectionsVariant>
{
public:
    ProjectionValueIterator() = default;
    explicit ProjectionValueIterator(const SingleThreadedCPUBackend::ProjectionContainer::const_iterator &iter)
        : iter_(iter)
    {
    }

    bool operator==(const BaseValueIterator<core::AllProjectionsVariant> &rhs) const override
    {
        if (typeid(*this) != typeid(rhs)) return false;
        return dynamic_cast<const ProjectionValueIterator &>(rhs).iter_ == iter_;
    }
    BaseValueIterator<core::AllProjectionsVariant> &operator++() override
    {
        ++iter_;
        return *this;
    }
    core::AllProjectionsVariant operator*() const override { return iter_->arg_; }

private:
    SingleThreadedCPUBackend::ProjectionContainer::const_iterator iter_;
};


core::Backend::DataRanges SingleThreadedCPUBackend::get_network_data() const
{
    using PopIterPtr = std::unique_ptr<BaseValueIterator<core::AllPopulationsVariant>>;
    using ProjIterPtr = std::unique_ptr<BaseValueIterator<core::AllProjectionsVariant>>;

    PopIterPtr pop_begin = std::make_unique<PopulationValueIterator>(PopulationValueIterator{populations_.begin()});
    PopIterPtr pop_end = std::make_unique<PopulationValueIterator>(PopulationValueIterator{populations_.end()});
    auto pop_range = std::make_pair(std::move(pop_begin), std::move(pop_end));

    ProjIterPtr proj_begin = std::make_unique<ProjectionValueIterator>(ProjectionValueIterator{projections_.begin()});
    ProjIterPtr proj_end = std::make_unique<ProjectionValueIterator>(ProjectionValueIterator{projections_.end()});
    auto proj_range = std::make_pair(std::move(proj_begin), std::move(proj_end));
    return DataRanges{std::move(proj_range), std::move(pop_range)};
}
}  // namespace knp::backends::single_threaded_cpu
