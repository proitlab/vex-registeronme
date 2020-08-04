#include <eosio/asset.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/eosio.hpp>


using namespace eosio;

class [[eosio::contract]] registeronme : public eosio::contract {
    
    using contract::contract;

    private:
        static constexpr symbol vex_symbol = symbol(symbol_code("VEX"), 4);
        //static constexpr symbol ram_symbol     = symbol(symbol_code("RAM"), 0);

    public:

        // @abi table member
        struct [[eosio::table]] member {
            name account_name;
            asset quantity;
            //uint64_t quantity;


            uint64_t primary_key()const { return account_name.value; }

            EOSLIB_SERIALIZE( member, (account_name)(quantity) )
        };

        typedef eosio::multi_index< "members"_n, member > members;


        /*
        uint32_t now() {
            return current_time_point().sec_since_epoch();
        }
        */

        [[eosio::action]]
        void getversion() {
            print("RegisterOnMe SC v1.7 - databisnisid - 20200804\t");
        }

        [[eosio::action]]
        void clearmembers() {
            require_auth(_self);

            members _members(get_self(), get_self().value);

            auto itr = _members.begin();
            while (itr != _members.end()) {
                itr = _members.erase(itr);
            }
            print("All records are deleted!");
        }
   
        [[eosio::on_notify("vex.token::transfer")]]
        void upsert(name from, name to, eosio::asset quantity, std::string memo) {
    
            if (to != get_self() || from == get_self())
            {
                print("What are you doing?");
                return;
            }

            if ( to == get_self()) {
                //check(now() < the_party, "You're way late");
                check(quantity.amount >= 10000, "Minimum transfer is 1 VEX");
                check(quantity.symbol == vex_symbol, "We only accept VEX.");

                members _members(get_self(), get_self().value);

                auto itr = _members.find( from.value );

                if( itr != _members.end() ) {
                    _members.modify( itr, get_self(), [&](auto &row) {
                        row.quantity.amount += quantity.amount;
                        row.quantity.symbol = quantity.symbol;
                    });
                } else {
                    _members.emplace( get_self(), [&](auto &row){
                        row.account_name = from;
                        row.quantity.amount = quantity.amount;
                        row.quantity.symbol = quantity.symbol;
                    });
                }
            }
            
       };
    

};