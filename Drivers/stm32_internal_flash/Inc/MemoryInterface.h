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
#include <functional>
#include <string.h>

namespace stm32_internal_flash {

namespace PropertyTypes {

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

} // namespace PropertyTypes

template <class ... Types> class PropertiesBase
{
public:
	using properties_t = std::variant<Types...>;
	using properties_storage_t = std::tuple<Types...>;
	using properties_changed_func_t = std::function<void(const properties_storage_t & old_properties)>;

public:
	properties_storage_t properties{};
	properties_changed_func_t properties_changed_func = []( const properties_storage_t & old_properties ) {};

public:

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

		properties_changed_func( old_properties );
	}

	void set( const properties_storage_t & new_properties ) {
		properties_storage_t old_properties = properties;
		properties = new_properties;

		properties_changed_func( old_properties );
	}

	template<class Prop>
	Prop::type_t get() const {
		return std::get<std::remove_cvref_t<Prop>>(properties).value;
	}
};


class MemoryInterfaceBase
{
public:
	struct Property
	{

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
		struct RestoreDataOnUnaligendWrites : public PropertyTypes::PropertyValueBooleanDefaultTrue {};

		/**
		 * Set to true if the driver supports this feature
		 */
		struct CanRestoreDataOnUnaligendWrites : public PropertyTypes::PropertyValueBooleanDefaultTrue {};

		/**
		 * Disable automatically page erase
		 */
		struct AutoErasePage : public PropertyTypes::PropertyValueBooleanDefaultTrue {};
	};
};

class MemoryInterface :  public MemoryInterfaceBase, public PropertiesBase<MemoryInterfaceBase::Property::RestoreDataOnUnaligendWrites,
											  	  	  	  	  	  	  	   MemoryInterfaceBase::Property::CanRestoreDataOnUnaligendWrites,
																		   MemoryInterfaceBase::Property::AutoErasePage>
{
public:

	MemoryInterface() {
		properties_changed_func = [this](  const properties_storage_t & old_properties ) {
			properties_changed( old_properties );
		};
	}

	virtual ~MemoryInterface() {}

	virtual std::size_t get_size() const = 0;

	virtual std::size_t get_page_size() const = 0;

	virtual std::size_t write( std::size_t address, const std::span<const std::byte> & data ) = 0;

	virtual std::size_t read( std::size_t address, std::span<std::byte> & data ) = 0;

	virtual bool erase( std::size_t address, std::size_t size ) = 0;


	virtual void properties_changed( properties_storage_t old_properties ) {
	}
};

} // namespace smt32_internal_flash

#endif /* APP_STM32_INTERNAL_FLASH_INC_MEMORYINTERFACE_H_ */
