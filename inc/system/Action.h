//
//  Action.h
//  common
//
//  Created by baowei on 2023/6/23.
//  Copyright (c) 2023 com. All rights reserved.
//

#ifndef Action_h
#define Action_h

#include <memory>
#include <tuple>
#include <utility>
#include <functional>
#include "system/OsDefine.h"

namespace System {
    template<size_t...>
    struct tuple_indices {
    };

    template<class IdxType, IdxType... Values>
    struct integer_sequence {
        template<template<class OIdxType, OIdxType...> class ToIndexSeq, class ToIndexType>
        using convert = ToIndexSeq<ToIndexType, Values...>;

        template<size_t Sp>
        using to_tuple_indices = tuple_indices<(Values + Sp)...>;
    };

    template<typename Tp, size_t ...Extra>
    struct repeat;
    template<typename Tp, Tp ...Np, size_t ...Extra>
    struct repeat<integer_sequence<Tp, Np...>, Extra...> {
        typedef integer_sequence<Tp,
                Np...,
                sizeof...(Np) + Np...,
                2 * sizeof...(Np) + Np...,
                3 * sizeof...(Np) + Np...,
                4 * sizeof...(Np) + Np...,
                5 * sizeof...(Np) + Np...,
                6 * sizeof...(Np) + Np...,
                7 * sizeof...(Np) + Np...,
                Extra...> type;
    };

    template<size_t Np>
    struct parity;
    template<size_t Np>
    struct make : parity<Np % 8>::template pmake<Np> {
    };

    template<>
    struct make<0> {
        typedef integer_sequence<size_t> type;
    };
    template<>
    struct make<1> {
        typedef integer_sequence<size_t, 0> type;
    };
    template<>
    struct make<2> {
        typedef integer_sequence<size_t, 0, 1> type;
    };
    template<>
    struct make<3> {
        typedef integer_sequence<size_t, 0, 1, 2> type;
    };
    template<>
    struct make<4> {
        typedef integer_sequence<size_t, 0, 1, 2, 3> type;
    };
    template<>
    struct make<5> {
        typedef integer_sequence<size_t, 0, 1, 2, 3, 4> type;
    };
    template<>
    struct make<6> {
        typedef integer_sequence<size_t, 0, 1, 2, 3, 4, 5> type;
    };
    template<>
    struct make<7> {
        typedef integer_sequence<size_t, 0, 1, 2, 3, 4, 5, 6> type;
    };

    template<>
    struct parity<0> {
        template<size_t Np>
        struct pmake : repeat<typename make<Np / 8>::type> {
        };
    };
    template<>
    struct parity<1> {
        template<size_t Np>
        struct pmake : repeat<typename make<Np / 8>::type, Np - 1> {
        };
    };
    template<>
    struct parity<2> {
        template<size_t Np>
        struct pmake : repeat<typename make<Np / 8>::type, Np - 2, Np - 1> {
        };
    };
    template<>
    struct parity<3> {
        template<size_t Np>
        struct pmake : repeat<typename make<Np / 8>::type, Np - 3, Np - 2, Np - 1> {
        };
    };
    template<>
    struct parity<4> {
        template<size_t Np>
        struct pmake : repeat<typename make<Np / 8>::type, Np - 4, Np - 3, Np - 2, Np - 1> {
        };
    };
    template<>
    struct parity<5> {
        template<size_t Np>
        struct pmake : repeat<typename make<Np / 8>::type,
                Np - 5, Np - 4, Np - 3, Np - 2, Np - 1> {
        };
    };
    template<>
    struct parity<6> {
        template<size_t Np>
        struct pmake : repeat<typename make<Np / 8>::type,
                Np - 6, Np - 5, Np - 4, Np - 3, Np - 2, Np - 1> {
        };
    };
    template<>
    struct parity<7> {
        template<size_t Np>
        struct pmake : repeat<typename make<Np / 8>::type,
                Np - 7, Np - 6, Np - 5, Np - 4, Np - 3, Np - 2, Np - 1> {
        };
    };

    template<size_t Ep, size_t Sp>
    using make_indices_imp =
            typename make<Ep - Sp>::type::template to_tuple_indices<Sp>;

    template<size_t Ep, size_t Sp = 0>
    struct make_tuple_indices {
        static_assert(Sp <= Ep, "make_tuple_indices input error");
        typedef make_indices_imp<Ep, Sp> type;
    };

    template<class ResultType>
    class Func {
    public:
        template<class Function, class... Args>
        explicit Func(Function &&f, Args &&... args) {
            typedef std::tuple<typename std::decay<Function>::type, typename std::decay<Args>::type...> Gp;
            std::unique_ptr<Gp> gp(
                    new Gp(std::forward<Function>(f),
                           std::forward<Args>(args)...));
            _action = &executeProxy<Gp>;
            _gp = gp.get();
            gp.release();

            _deleteGp = &Func::deleteGp<Gp>;
        }

        Func(const Func &) = delete;

        virtual ~Func() {
            _action = nullptr;
            (this->*_deleteGp)();
            _deleteGp = nullptr;
        }

        ResultType execute() {
            return _action(_gp);
        }

        ResultType operator()() {
            return execute();
        };

    public:
#if __cplusplus > 201402L

#define invoke std::invoke

#else

        template<typename Functor, typename... Args>
        static typename std::enable_if<
                std::is_member_pointer<typename std::decay<Functor>::type>::value,
                typename std::result_of<Functor &&(Args &&...)>::type
        >::type invoke(Functor &&f, Args &&... args) {
            return std::mem_fn(f)(std::forward<Args>(args)...);
        }

        template<typename Functor, typename... Args>
        static typename std::enable_if<
                !std::is_member_pointer<typename std::decay<Functor>::type>::value,
                typename std::result_of<Functor &&(Args &&...)>::type
        >::type invoke(Functor &&f, Args &&... args) {
            return std::forward<Functor>(f)(std::forward<Args>(args)...);
        }

#endif // C++17

    private:
        template<class Gp>
        static ResultType executeProxy(void *pgp) {
            auto emptyDeleter = [](Gp *gp) -> void {
            };
            std::unique_ptr<Gp, decltype(emptyDeleter)> gp(static_cast<Gp *>(pgp), emptyDeleter);
            typedef typename make_tuple_indices<std::tuple_size<Gp>::value, 1>::type Index;
            return executeProxy(*gp, Index());
        }

        template<class Function, class ...Args, size_t ...Indices>
        static ResultType executeProxy(std::tuple<Function, Args...> &gp, tuple_indices<Indices...>) {
            return invoke(std::move(std::get<0>(gp)), std::move(std::get<Indices>(gp))...);
        }

        template<class Gp>
        void deleteGp() {
            std::unique_ptr<Gp> gp(static_cast<Gp *>(_gp));
            Gp *pgp = gp.release();
            delete pgp;
        }

    private:
        typedef ResultType (*Callback)(void *);

        Callback _action;
        void *_gp;

        typedef void (Func::*DeleteCallback)();

        DeleteCallback _deleteGp;
    };

    typedef Func<void> Action;
}

#endif // Action_h
