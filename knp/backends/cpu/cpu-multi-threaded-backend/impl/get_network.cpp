/**
 * @file get_network.cpp
 * @brief Getting network data from multi-threaded CPU backend.
 * @author An. Vartenkov.
 * @date 20.05.2024
 */

#include <knp/backends/cpu-multi-threaded/backend.h>
#include <knp/meta/variant_helpers.h>


namespace knp::backends::multi_threaded_cpu
{
class PopulationValueIterator : public MultiThreadedCPUBackend::BaseValueIterator<core::AllPopulationsVariant>
{
public:
    PopulationValueIterator() = default;
    explicit PopulationValueIterator(const MultiThreadedCPUBackend::PopulationContainer::const_iterator &it) : it_(it)
    {
    }
    bool operator==(const BaseValueIterator<core::AllPopulationsVariant> &rhs) const override
    {
        if (typeid(*this) != typeid(rhs)) return false;
        return dynamic_cast<const PopulationValueIterator &>(rhs).it_ == it_;
    }

    BaseValueIterator<core::AllPopulationsVariant> &operator++() override
    {
        ++it_;
        return *this;
    }
    core::AllPopulationsVariant operator*() const override { return knp::meta::variant_cast(*it_); }

private:
    MultiThreadedCPUBackend::PopulationContainer::const_iterator it_;
};


class ProjectionValueIterator : public MultiThreadedCPUBackend::BaseValueIterator<core::AllProjectionsVariant>
{
public:
    ProjectionValueIterator() = default;
    explicit ProjectionValueIterator(const MultiThreadedCPUBackend::ProjectionContainer::const_iterator &it) : it_(it)
    {
    }

    bool operator==(const BaseValueIterator<core::AllProjectionsVariant> &rhs) const override
    {
        if (typeid(*this) != typeid(rhs)) return false;
        return dynamic_cast<const ProjectionValueIterator &>(rhs).it_ == it_;
    }
    BaseValueIterator<core::AllProjectionsVariant> &operator++() override
    {
        ++it_;
        return *this;
    }
    core::AllProjectionsVariant operator*() const override { return knp::meta::variant_cast(it_->arg_); }

private:
    MultiThreadedCPUBackend::ProjectionContainer::const_iterator it_;
};


core::Backend::DataRanges MultiThreadedCPUBackend::get_model_data() const
{
    using PopIterPtr = std::unique_ptr<BaseValueIterator<core::AllPopulationsVariant>>;
    using ProjIterPtr = std::unique_ptr<BaseValueIterator<core::AllProjectionsVariant>>;

    PopIterPtr pop_begin = std::make_unique<PopulationValueIterator>(PopulationValueIterator{populations_.begin()});
    PopIterPtr pop_end = std::make_unique<PopulationValueIterator>(PopulationValueIterator{populations_.end()});
    auto pop_range = std::make_pair(std::move(pop_begin), std::move(pop_end));

    ProjIterPtr proj_begin = std::make_unique<ProjectionValueIterator>(ProjectionValueIterator{projections_.begin()});
    ProjIterPtr proj_end = std::make_unique<ProjectionValueIterator>(ProjectionValueIterator{projections_.end()});
    auto proj_range = std::make_pair(std::move(proj_begin), std::move(proj_end));
    return DataRanges{.projection_range{std::move(proj_range)}, .population_range{std::move(pop_range)}};
}
}  // namespace knp::backends::multi_threaded_cpu
