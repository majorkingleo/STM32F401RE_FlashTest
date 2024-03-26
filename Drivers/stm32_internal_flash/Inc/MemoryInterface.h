/*
 * Exposed MemoryInterface of this driver
 * @author Copyright (c) 2024 Martin Oberzalek
 */

#ifndef APP_STM32_INTERNAL_FLASH_INC_MEMORYINTERFACE_H_
#define APP_STM32_INTERNAL_FLASH_INC_MEMORYINTERFACE_H_

#include <cstddef>
#include <span>
#include <type_traits>
#include <tuple>
#include <variant>
#include <string.h>

namespace stm32_internal_flash {

class MemoryInterface
{
public:
	struct Property
	{
		template<class Value> struct PropertyValue
		{
			using type_t = Value;
			type_t value{};
		};

		struct PropertyValueBooleanDefaultTrue : public PropertyValue<bool> {
			PropertyValueBooleanDefaultTrue( bool value_ = true ) {
				value = value_;
			}
		};


		/*
		struct PropertyValueBooleanDefaultTrue {
			using type_t = bool;
			type_t value;

			PropertyValueBooleanDefaultTrue( bool value_ = true ) {
				value = value_;
			}
		};
		*/

		/**
		 * Since the flash drive can erase only whole pages,
		 * It's not possible writing only half of a page, without destroying all.
		 * To do this the driver will read the whole page before, manipulates the data
		 * in RAM, erases the page and writes back all.
		 *
		 * This requires at least PAGE_SIZE bytes of RAM.
		 *
		 * If you don't have so much RAM, or just don't need it you can disable
		 * this feature.
		 */
		struct RestoreDataOnUnaligendWrites : public PropertyValueBooleanDefaultTrue {};

		/**
		 * Set to true if the driver supports this feature
		 */
		struct CanRestoreDataOnUnaligendWrites : public PropertyValueBooleanDefaultTrue {};

		/**
		 * Disable automatically page erase
		 */
		struct AutoErasePage : public PropertyValueBooleanDefaultTrue {};
	};

	using properties_t = std::variant<Property::AutoErasePage,
									  Property::RestoreDataOnUnaligendWrites,
									  Property::CanRestoreDataOnUnaligendWrites>;

	using properties_storage_t = std::tuple<Property::AutoErasePage,
									  	    Property::RestoreDataOnUnaligendWrites,
											Property::CanRestoreDataOnUnaligendWrites>;

protected:
	properties_storage_t properties;

public:

	virtual ~MemoryInterface() {}

	virtual std::size_t get_size() const = 0;

	virtual std::size_t get_page_size() const = 0;

	virtual std::size_t write( std::size_t address, const std::span<const std::byte> & data ) = 0;

	std::size_t write( std::size_t address, const std::span<const char> & data ) {
		return write( address, std::span<const std::byte>(reinterpret_cast<const std::byte*>(data.data()), data.size()));
	}

	virtual std::size_t read( std::size_t address, std::span<std::byte> & data ) = 0;

	virtual bool erase( std::size_t address, std::size_t size ) = 0;



	template<class Prop>
	void set( const Prop & prop ) {
		std::get<Prop>(properties).value = prop.value;
	}

	void set( const std::initializer_list<properties_t> & props ) {

		properties_storage_t old_properties = properties;

		for( const auto & prop : props ) {
			std::visit([this]( const auto & p ) {
				set(p);
			}, prop );
		}

		properties_changed( old_properties );
	}

	void set( const properties_storage_t & new_properties ) {
		properties_storage_t old_properties = properties;
		properties = new_properties;

		properties_changed( old_properties );
	}

	template<class Prop>
	Prop::type_t get() const {
		return std::get<std::remove_cvref_t<Prop>>(properties).value;
	}

	virtual void properties_changed( properties_storage_t old_properties ) {
	}
};

} // namespace smt32_internal_flash

#endif /* APP_STM32_INTERNAL_FLASH_INC_MEMORYINTERFACE_H_ */
