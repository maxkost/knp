/**
 * @file backend.cpp
 * @brief Common backend class Python bindings.
 * @author Artiom N.
 * @date 01.02.2024
 */
#if defined(_KNP_IN_CORE)

struct BackendWrapper : core::Backend, py::wrapper<core::Backend>
{
    ~BackendWrapper() = default;
    bool plasticity_supported() const override { return this->get_override("plasticity_supported")(); }

    std::vector<std::string> get_supported_neurons() const override
    {
        return this->get_override("get_supported_neurons")();
    }

    std::vector<std::string> get_supported_synapses() const override
    {
        return this->get_override("get_supported_synapses")();
    }

    std::vector<size_t> get_supported_population_indexes() const override
    {
        return this->get_override("get_supported_population_indexes")();
    }

    std::vector<size_t> get_supported_projection_indexes() const override
    {
        return this->get_override("get_supported_projection_indexes")();
    }

    void load_all_projections(const std::vector<core::AllProjectionsVariant> &projections) override
    {
        this->get_override("load_all_projections")(projections);
    }

    void load_all_populations(const std::vector<core::AllPopulationsVariant> &populations) override
    {
        this->get_override("load_all_populations")(populations);
    }

    void remove_projections(const std::vector<core::UID> &uids) override
    {
        this->get_override("remove_projections")(uids);
    }

    void remove_populations(const std::vector<core::UID> &uids) override
    {
        this->get_override("remove_populations")(uids);
    }
    std::vector<std::unique_ptr<core::Device>> get_devices() const override
    {
        // return py::call<std::vector<std::unique_ptr<core::Device>>>(this->get_override("get_devices").ptr());
        return {};
    }
    std::vector<std::unique_ptr<knp::core::Device>> &get_current_devices()
    {
        return this->get_override("get_current_devices")();
    }
    const std::vector<std::unique_ptr<core::Device>> &get_current_devices() const
    {
        return this->get_override("get_current_devices")();
    }
    void select_devices(const std::set<core::UID> &uids) override { this->get_override("select_devices")(uids); }
    const core::MessageEndpoint &get_message_endpoint() const override
    {
        return this->get_override("get_message_endpoint")();
    }
    core::MessageEndpoint &get_message_endpoint() override { return this->get_override("get_message_endpoint")(); }
    void stop_learning() override { this->get_override("stop_learning")(); }
    void start_learning() override { this->get_override("start_learning")(); }
    void _step() override { this->get_override("step")(); }
    void _init() override { this->get_override("init")(); }
};


// py::class_<std::vector<core::AllPopulationsVariant>>("AllPopulations")
//     .def(py::vector_indexing_suite<std::vector<core::AllPopulationsVariant>>() );

// py::class_<std::vector<core::AllProjectionsVariant>>("AllProjections")
//     .def(py::vector_indexing_suite<std::vector<core::AllProjectionsVariant>>() );


// Abstract class
py::class_<BackendWrapper, boost::noncopyable>(
    "Backend", "The Backend class is the base class for backends.", py::no_init)
    .def(
        "load_all_populations",
        make_handler(
            [](core::Backend &b, const py::list &l)
            {
                using PT = core::AllPopulationsVariant;
                b.load_all_populations(std::vector<PT>(py::stl_input_iterator<PT>(l), py::stl_input_iterator<PT>()));
            }),
        "Add populations to backend.")
    .def(
        "load_all_projections",
        make_handler(
            [](core::Backend &b, const py::list &l)
            {
                using PT = core::AllProjectionsVariant;
                b.load_all_projections(std::vector<PT>(py::stl_input_iterator<PT>(l), py::stl_input_iterator<PT>()));
            }),
        "Add projections to backend.")
    .def("load_all_projections", py::pure_virtual(&core::Backend::load_all_projections), "Add projections to backend.")
    .def(
        "remove_projections", py::pure_virtual(&core::Backend::remove_projections),
        "Remove projections with given UIDs from the backend.")
    .def(
        "remove_populations", py::pure_virtual(&core::Backend::remove_populations),
        "Remove populations with given UIDs from the backend.")
    //    .def("get_current_devices",
    //    py::pure_virtual(static_cast<std::vector<std::unique_ptr<core::Device>>&(core::Backend::*)()>(&core::Backend::get_current_devices)),
    //         "Get a list of devices on which the backend runs a network.")
    //    .def("get_current_devices", py::pure_virtual(static_cast<const
    //    std::vector<std::unique_ptr<core::Device>>&(core::Backend::*) const ()>(&core::Backend::get_current_devices)),
    //         "Get a list of devices on which the backend runs a network.")
    .def(
        "select_devices", py::pure_virtual(&core::Backend::select_devices),
        "Select devices on which to run the backend.")
    //    .def( "get_message_endpoint",
    //    py::pure_virtual(static_cast<core::MessageEndpoint&(core::Backend::*)()>(&core::Backend::get_message_endpoint)),
    //    "Get message endpoint.") .def("get_message_endpoint", static_cast<const
    //    core::MessageEndpoint&(core::Backend::*)() const>(& core::Backend::get_message_endpoint), "Get message
    //    endpoint.")
    .def(
        "start", static_cast<void (core::Backend::*)()>(&core::Backend::start),
        "Start network execution on the backend.")
    .def(
        "start",
        make_handler(
            [](core::Backend &b, py::object &run_predicate)
            {
                b.start(
                    [&run_predicate](core::Step step)
                    {
                        auto res = py::call<py::object>(run_predicate.ptr(), step);
                        return py::extract<bool>(res);
                    });
            }),
        "Start network execution on the backend.")
    .def(
        "start",
        make_handler(
            [](core::Backend &b, py::object &pre_step, py::object &post_step)
            {
                b.start(
                    [&pre_step](core::Step step)
                    {
                        auto res = py::call<py::object>(pre_step.ptr(), step);
                        return py::extract<bool>(res);
                    },
                    [&post_step](core::Step step)
                    {
                        auto res = py::call<py::object>(post_step.ptr(), step);
                        return py::extract<bool>(res);
                    });
            }),
        "Start network execution on the backend.")
    .def("stop", &core::Backend::stop, "Stop network execution on the backend.")
    .def("get_step", &core::Backend::get_step, "Get current step.")
    .def("stop_learning", &core::Backend::stop_learning, "Stop learning.")
    .def("start_learning", &core::Backend::start_learning, "Restart learning.")
    .def("_init", &core::Backend::_init, "Initialize backend before starting network execution.")
    .def("_step", &core::Backend::_step, "Make one network execution step.")
    .def("_uninit", &core::Backend::_uninit, "Set backend to the uninitialized state.")
    .def_readonly("message_bus", &core::Backend::message_bus_, "Get message bus")
    .add_property("uid", make_handler([](core::Backend &b) { return b.get_uid(); }), "Get backend UID.")
    .add_property("running", &core::Backend::running, "Get network execution status.");

#endif
