API
===

.. class:: ctll::fixed_string

  *Some documentation*

  Example: ::

    static constexpr auto pattern = ctll::fixed_string{ "h.*" };
  
    constexpr auto match(std::string_view sv) noexcept {
    	return ctre::match<pattern>(sv);
    }

.. class:: template<class Iterator, class... Captures> ctre::regex_results

  .. function:: template<size_t Id> constexpr auto get()
                template<class Id> constexpr auto get()

    Returns the capture specified by ``Id``.

  .. function:: constexpr size_t size()
    
    Returns the number of captures in this result object.

.. function:: template<class... Args> constexpr ctre::regex_results<deduced> match(Args&&... args)

  *Some documentation*

.. function:: template<class... Args> constexpr ctre::regex_results<deduced> search(Args&&... args)

  *Some documentation*

*More functions and types*