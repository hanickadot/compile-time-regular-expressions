API
===

.. class:: ctll::fixed_string

  A compile-time fixed string.

  Example: ::

    static constexpr auto pattern = ctll::fixed_string{ "h.*" };
  
    constexpr auto match(std::string_view sv) noexcept {
    	return ctre::match<pattern>(sv);
    }

.. class:: template<class Iterator, class... Captures> ctre::regex_results

  .. type:: char_type = typename std::iterator_traits<Iterator>::value_type

    The character type used by the ``Iterator``.

  .. function:: template<size_t Id> constexpr captured_content<Id, void>::storage<Iterator> get()
                template<class Name> constexpr captured_content<deduced, Name>::storage<Iterator> get()
                template<ctll::fixed_string Name> constexpr captured_content<deduced, Name>::storage<Iterator> get()

    Returns the capture specified by ``Id`` or ``Name``. ID ``0`` is the full match, ID ``1`` is the first capture group, ID ``2`` is the second, etc.
    Named groups are specified using ``(?<name>)``.

    Example: ::

      if (auto m = ctre::match<"(?<chars>[a-z]+)([0-9]+)">("abc123")) {
        m.get<"chars">(); //abc
        m.get<2>(); //123
      }

  .. function:: constexpr size_t size()
    
    Returns the number of captures in this result object.

  .. function:: constexpr operator bool() const noexcept
    
    Returns whether the match was successful.

  .. function:: constexpr operator std::basic_string_view<char_type>() const noexcept
                constexpr std::basic_string_view<char_type> to_view() const noexcept
                constexpr std::basic_string_view<char_type> view() const noexcept

    Converts the match to a string view.

  .. function:: constexpr explicit operator std::basic_string<char_type>() const noexcept
                constexpr std::basic_string<char_type> to_string() const noexcept
                constexpr std::basic_string<char_type> str() const noexcept

    Converts the match to a string view. 

.. class:: template<size_t Id, typename Name = void> captured_content

   .. class:: template <typename Iterator> storage

      .. function:: constexpr auto begin() const noexcept
                    constexpr auto end() const noexcept

        Returns the begin or end iterator for the captured content.

      .. function:: constexpr operator bool() const noexcept

        Returns whether the match was successful.

      .. function:: constexpr auto size() const noexcept

        Returns the number of characters in the capture.

      .. function:: constexpr operator std::basic_string_view<char_type>() const noexcept
                    constexpr std::basic_string_view<char_type> to_view() const noexcept
                    constexpr std::basic_string_view<char_type> view() const noexcept

        Converts the capture to a string view.

      .. function:: constexpr explicit operator std::basic_string<char_type>() const noexcept
                    constexpr std::basic_string<char_type> to_string() const noexcept
                    constexpr std::basic_string<char_type> str() const noexcept

        Converts the capture to a string view. 

      .. function:: constexpr static size_t get_id() noexcept

        Returns ``Id``

.. function:: template<auto & RE, class... Args> constexpr ctre::regex_results<deduced> match(Args&&... args)
              template<ctll::fixed_string RE, class... Args> constexpr ctre::regex_results<deduced> match(Args&&... args)

  Matches ``RE`` against the whole input.
  ``Args...`` must be either a string-like object with ``begin`` and ``end`` member functions, or a pair of forward iterators. 

.. function:: template<auto & RE, class... Args> constexpr ctre::regex_results<deduced> search(Args&&... args)
              template<ctll::fixed_string RE, class... Args>  constexpr ctre::regex_results<deduced> search(Args&&... args)

  Searches for a match somewhere within the input.
  ``Args...`` must be either a string-like object with ``begin`` and ``end`` member functions, or a pair of forward iterators.
  
