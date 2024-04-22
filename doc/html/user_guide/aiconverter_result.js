/*!
 * https://github.com/es-shims/es5-shim
 * @license es5-shim Copyright 2009-2015 by contributors, MIT License
 * see https://github.com/es-shims/es5-shim/blob/master/LICENSE
 */

// vim: ts=4 sts=4 sw=4 expandtab

// Add semicolon to prevent IIFE from being passed as argument to concatenated code.
;

// UMD (Universal Module Definition)
// see https://github.com/umdjs/umd/blob/master/templates/returnExports.js
(function (root, factory) {
    'use strict';

    /* global define, exports, module */
    if (typeof define === 'function' && define.amd) {
        // AMD. Register as an anonymous module.
        define(factory);
    } else if (typeof exports === 'object') {
        // Node. Does not work with strict CommonJS, but
        // only CommonJS-like enviroments that support module.exports,
        // like Node.
        module.exports = factory();
    } else {
        // Browser globals (root is window)
        root.returnExports = factory();
    }
}(this, function () {

/**
 * Brings an environment as close to ECMAScript 5 compliance
 * as is possible with the facilities of erstwhile engines.
 *
 * Annotated ES5: http://es5.github.com/ (specific links below)
 * ES5 Spec: http://www.ecma-international.org/publications/files/ECMA-ST/Ecma-262.pdf
 * Required reading: http://javascriptweblog.wordpress.com/2011/12/05/extending-javascript-natives/
 */

// Shortcut to an often accessed properties, in order to avoid multiple
// dereference that costs universally. This also holds a reference to known-good
// functions.
var $Array = Array;
var ArrayPrototype = $Array.prototype;
var $Object = Object;
var ObjectPrototype = $Object.prototype;
var $Function = Function;
var FunctionPrototype = $Function.prototype;
var $String = String;
var StringPrototype = $String.prototype;
var $Number = Number;
var NumberPrototype = $Number.prototype;
var array_slice = ArrayPrototype.slice;
var array_splice = ArrayPrototype.splice;
var array_push = ArrayPrototype.push;
var array_unshift = ArrayPrototype.unshift;
var array_concat = ArrayPrototype.concat;
var array_join = ArrayPrototype.join;
var call = FunctionPrototype.call;
var apply = FunctionPrototype.apply;
var max = Math.max;
var min = Math.min;

// Having a toString local variable name breaks in Opera so use to_string.
var to_string = ObjectPrototype.toString;

/* global Symbol */
/* eslint-disable one-var-declaration-per-line, no-redeclare */
var hasToStringTag = typeof Symbol === 'function' && typeof Symbol.toStringTag === 'symbol';
var isCallable; /* inlined from https://npmjs.com/is-callable */ var fnToStr = Function.prototype.toString, constructorRegex = /^\s*class /, isES6ClassFn = function isES6ClassFn(value) { try { var fnStr = fnToStr.call(value); var singleStripped = fnStr.replace(/\/\/.*\n/g, ''); var multiStripped = singleStripped.replace(/\/\*[.\s\S]*\*\//g, ''); var spaceStripped = multiStripped.replace(/\n/mg, ' ').replace(/ {2}/g, ' '); return constructorRegex.test(spaceStripped); } catch (e) { return false; /* not a function */ } }, tryFunctionObject = function tryFunctionObject(value) { try { if (isES6ClassFn(value)) { return false; } fnToStr.call(value); return true; } catch (e) { return false; } }, fnClass = '[object Function]', genClass = '[object GeneratorFunction]', isCallable = function isCallable(value) { if (!value) { return false; } if (typeof value !== 'function' && typeof value !== 'object') { return false; } if (hasToStringTag) { return tryFunctionObject(value); } if (isES6ClassFn(value)) { return false; } var strClass = to_string.call(value); return strClass === fnClass || strClass === genClass; };

var isRegex; /* inlined from https://npmjs.com/is-regex */ var regexExec = RegExp.prototype.exec, tryRegexExec = function tryRegexExec(value) { try { regexExec.call(value); return true; } catch (e) { return false; } }, regexClass = '[object RegExp]'; isRegex = function isRegex(value) { if (typeof value !== 'object') { return false; } return hasToStringTag ? tryRegexExec(value) : to_string.call(value) === regexClass; };
var isString; /* inlined from https://npmjs.com/is-string */ var strValue = String.prototype.valueOf, tryStringObject = function tryStringObject(value) { try { strValue.call(value); return true; } catch (e) { return false; } }, stringClass = '[object String]'; isString = function isString(value) { if (typeof value === 'string') { return true; } if (typeof value !== 'object') { return false; } return hasToStringTag ? tryStringObject(value) : to_string.call(value) === stringClass; };
/* eslint-enable one-var-declaration-per-line, no-redeclare */

/* inlined from http://npmjs.com/define-properties */
var supportsDescriptors = $Object.defineProperty && (function () {
    try {
        var obj = {};
        $Object.defineProperty(obj, 'x', { enumerable: false, value: obj });
        for (var _ in obj) { return false; }
        return obj.x === obj;
    } catch (e) { /* this is ES3 */
        return false;
    }
}());
var defineProperties = (function (has) {
  // Define configurable, writable, and non-enumerable props
  // if they don't exist.
  var defineProperty;
  if (supportsDescriptors) {
      defineProperty = function (object, name, method, forceAssign) {
          if (!forceAssign && (name in object)) { return; }
          $Object.defineProperty(object, name, {
              configurable: true,
              enumerable: false,
              writable: true,
              value: method
          });
      };
  } else {
      defineProperty = function (object, name, method, forceAssign) {
          if (!forceAssign && (name in object)) { return; }
          object[name] = method;
      };
  }
  return function defineProperties(object, map, forceAssign) {
      for (var name in map) {
          if (has.call(map, name)) {
            defineProperty(object, name, map[name], forceAssign);
          }
      }
  };
}(ObjectPrototype.hasOwnProperty));

//
// Util
// ======
//

/* replaceable with https://npmjs.com/package/es-abstract /helpers/isPrimitive */
var isPrimitive = function isPrimitive(input) {
    var type = typeof input;
    return input === null || (type !== 'object' && type !== 'function');
};

var isActualNaN = $Number.isNaN || function (x) { return x !== x; };

var ES = {
    // ES5 9.4
    // http://es5.github.com/#x9.4
    // http://jsperf.com/to-integer
    /* replaceable with https://npmjs.com/package/es-abstract ES5.ToInteger */
    ToInteger: function ToInteger(num) {
        var n = +num;
        if (isActualNaN(n)) {
            n = 0;
        } else if (n !== 0 && n !== (1 / 0) && n !== -(1 / 0)) {
            n = (n > 0 || -1) * Math.floor(Math.abs(n));
        }
        return n;
    },

    /* replaceable with https://npmjs.com/package/es-abstract ES5.ToPrimitive */
    ToPrimitive: function ToPrimitive(input) {
        var val, valueOf, toStr;
        if (isPrimitive(input)) {
            return input;
        }
        valueOf = input.valueOf;
        if (isCallable(valueOf)) {
            val = valueOf.call(input);
            if (isPrimitive(val)) {
                return val;
            }
        }
        toStr = input.toString;
        if (isCallable(toStr)) {
            val = toStr.call(input);
            if (isPrimitive(val)) {
                return val;
            }
        }
        throw new TypeError();
    },

    // ES5 9.9
    // http://es5.github.com/#x9.9
    /* replaceable with https://npmjs.com/package/es-abstract ES5.ToObject */
    ToObject: function (o) {
        if (o == null) { // this matches both null and undefined
            throw new TypeError("can't convert " + o + ' to object');
        }
        return $Object(o);
    },

    /* replaceable with https://npmjs.com/package/es-abstract ES5.ToUint32 */
    ToUint32: function ToUint32(x) {
        return x >>> 0;
    }
};

//
// Function
// ========
//

// ES-5 15.3.4.5
// http://es5.github.com/#x15.3.4.5

var Empty = function Empty() {};

defineProperties(FunctionPrototype, {
    bind: function bind(that) { // .length is 1
        // 1. Let Target be the this value.
        var target = this;
        // 2. If IsCallable(Target) is false, throw a TypeError exception.
        if (!isCallable(target)) {
            throw new TypeError('Function.prototype.bind called on incompatible ' + target);
        }
        // 3. Let A be a new (possibly empty) internal list of all of the
        //   argument values provided after thisArg (arg1, arg2 etc), in order.
        // XXX slicedArgs will stand in for "A" if used
        var args = array_slice.call(arguments, 1); // for normal call
        // 4. Let F be a new native ECMAScript object.
        // 11. Set the [[Prototype]] internal property of F to the standard
        //   built-in Function prototype object as specified in 15.3.3.1.
        // 12. Set the [[Call]] internal property of F as described in
        //   15.3.4.5.1.
        // 13. Set the [[Construct]] internal property of F as described in
        //   15.3.4.5.2.
        // 14. Set the [[HasInstance]] internal property of F as described in
        //   15.3.4.5.3.
        var bound;
        var binder = function () {

            if (this instanceof bound) {
                // 15.3.4.5.2 [[Construct]]
                // When the [[Construct]] internal method of a function object,
                // F that was created using the bind function is called with a
                // list of arguments ExtraArgs, the following steps are taken:
                // 1. Let target be the value of F's [[TargetFunction]]
                //   internal property.
                // 2. If target has no [[Construct]] internal method, a
                //   TypeError exception is thrown.
                // 3. Let boundArgs be the value of F's [[BoundArgs]] internal
                //   property.
                // 4. Let args be a new list containing the same values as the
                //   list boundArgs in the same order followed by the same
                //   values as the list ExtraArgs in the same order.
                // 5. Return the result of calling the [[Construct]] internal
                //   method of target providing args as the arguments.

                var result = apply.call(
                    target,
                    this,
                    array_concat.call(args, array_slice.call(arguments))
                );
                if ($Object(result) === result) {
                    return result;
                }
                return this;

            } else {
                // 15.3.4.5.1 [[Call]]
                // When the [[Call]] internal method of a function object, F,
                // which was created using the bind function is called with a
                // this value and a list of arguments ExtraArgs, the following
                // steps are taken:
                // 1. Let boundArgs be the value of F's [[BoundArgs]] internal
                //   property.
                // 2. Let boundThis be the value of F's [[BoundThis]] internal
                //   property.
                // 3. Let target be the value of F's [[TargetFunction]] internal
                //   property.
                // 4. Let args be a new list containing the same values as the
                //   list boundArgs in the same order followed by the same
                //   values as the list ExtraArgs in the same order.
                // 5. Return the result of calling the [[Call]] internal method
                //   of target providing boundThis as the this value and
                //   providing args as the arguments.

                // equiv: target.call(this, ...boundArgs, ...args)
                return apply.call(
                    target,
                    that,
                    array_concat.call(args, array_slice.call(arguments))
                );

            }

        };

        // 15. If the [[Class]] internal property of Target is "Function", then
        //     a. Let L be the length property of Target minus the length of A.
        //     b. Set the length own property of F to either 0 or L, whichever is
        //       larger.
        // 16. Else set the length own property of F to 0.

        var boundLength = max(0, target.length - args.length);

        // 17. Set the attributes of the length own property of F to the values
        //   specified in 15.3.5.1.
        var boundArgs = [];
        for (var i = 0; i < boundLength; i++) {
            array_push.call(boundArgs, '$' + i);
        }

        // XXX Build a dynamic function with desired amount of arguments is the only
        // way to set the length property of a function.
        // In environments where Content Security Policies enabled (Chrome extensions,
        // for ex.) all use of eval or Function costructor throws an exception.
        // However in all of these environments Function.prototype.bind exists
        // and so this code will never be executed.
        bound = $Function('binder', 'return function (' + array_join.call(boundArgs, ',') + '){ return binder.apply(this, arguments); }')(binder);

        if (target.prototype) {
            Empty.prototype = target.prototype;
            bound.prototype = new Empty();
            // Clean up dangling references.
            Empty.prototype = null;
        }

        // TODO
        // 18. Set the [[Extensible]] internal property of F to true.

        // TODO
        // 19. Let thrower be the [[ThrowTypeError]] function Object (13.2.3).
        // 20. Call the [[DefineOwnProperty]] internal method of F with
        //   arguments "caller", PropertyDescriptor {[[Get]]: thrower, [[Set]]:
        //   thrower, [[Enumerable]]: false, [[Configurable]]: false}, and
        //   false.
        // 21. Call the [[DefineOwnProperty]] internal method of F with
        //   arguments "arguments", PropertyDescriptor {[[Get]]: thrower,
        //   [[Set]]: thrower, [[Enumerable]]: false, [[Configurable]]: false},
        //   and false.

        // TODO
        // NOTE Function objects created using Function.prototype.bind do not
        // have a prototype property or the [[Code]], [[FormalParameters]], and
        // [[Scope]] internal properties.
        // XXX can't delete prototype in pure-js.

        // 22. Return F.
        return bound;
    }
});

// _Please note: Shortcuts are defined after `Function.prototype.bind` as we
// use it in defining shortcuts.
var owns = call.bind(ObjectPrototype.hasOwnProperty);
var toStr = call.bind(ObjectPrototype.toString);
var arraySlice = call.bind(array_slice);
var arraySliceApply = apply.bind(array_slice);
var strSlice = call.bind(StringPrototype.slice);
var strSplit = call.bind(StringPrototype.split);
var strIndexOf = call.bind(StringPrototype.indexOf);
var pushCall = call.bind(array_push);
var isEnum = call.bind(ObjectPrototype.propertyIsEnumerable);
var arraySort = call.bind(ArrayPrototype.sort);

//
// Array
// =====
//

var isArray = $Array.isArray || function isArray(obj) {
    return toStr(obj) === '[object Array]';
};

// ES5 15.4.4.12
// http://es5.github.com/#x15.4.4.13
// Return len+argCount.
// [bugfix, ielt8]
// IE < 8 bug: [].unshift(0) === undefined but should be "1"
var hasUnshiftReturnValueBug = [].unshift(0) !== 1;
defineProperties(ArrayPrototype, {
    unshift: function () {
        array_unshift.apply(this, arguments);
        return this.length;
    }
}, hasUnshiftReturnValueBug);

// ES5 15.4.3.2
// http://es5.github.com/#x15.4.3.2
// https://developer.mozilla.org/en/JavaScript/Reference/Global_Objects/Array/isArray
defineProperties($Array, { isArray: isArray });

// The IsCallable() check in the Array functions
// has been replaced with a strict check on the
// internal class of the object to trap cases where
// the provided function was actually a regular
// expression literal, which in V8 and
// JavaScriptCore is a typeof "function".  Only in
// V8 are regular expression literals permitted as
// reduce parameters, so it is desirable in the
// general case for the shim to match the more
// strict and common behavior of rejecting regular
// expressions.

// ES5 15.4.4.18
// http://es5.github.com/#x15.4.4.18
// https://developer.mozilla.org/en/JavaScript/Reference/Global_Objects/array/forEach

// Check failure of by-index access of string characters (IE < 9)
// and failure of `0 in boxedString` (Rhino)
var boxedString = $Object('a');
var splitString = boxedString[0] !== 'a' || !(0 in boxedString);

var properlyBoxesContext = function properlyBoxed(method) {
    // Check node 0.6.21 bug where third parameter is not boxed
    var properlyBoxesNonStrict = true;
    var properlyBoxesStrict = true;
    var threwException = false;
    if (method) {
        try {
            method.call('foo', function (_, __, context) {
                if (typeof context !== 'object') { properlyBoxesNonStrict = false; }
            });

            method.call([1], function () {
                'use strict';

                properlyBoxesStrict = typeof this === 'string';
            }, 'x');
        } catch (e) {
            threwException = true;
        }
    }
    return !!method && !threwException && properlyBoxesNonStrict && properlyBoxesStrict;
};

defineProperties(ArrayPrototype, {
    forEach: function forEach(callbackfn/*, thisArg*/) {
        var object = ES.ToObject(this);
        var self = splitString && isString(this) ? strSplit(this, '') : object;
        var i = -1;
        var length = ES.ToUint32(self.length);
        var T;
        if (arguments.length > 1) {
          T = arguments[1];
        }

        // If no callback function or if callback is not a callable function
        if (!isCallable(callbackfn)) {
            throw new TypeError('Array.prototype.forEach callback must be a function');
        }

        while (++i < length) {
            if (i in self) {
                // Invoke the callback function with call, passing arguments:
                // context, property value, property key, thisArg object
                if (typeof T === 'undefined') {
                    callbackfn(self[i], i, object);
                } else {
                    callbackfn.call(T, self[i], i, object);
                }
            }
        }
    }
}, !properlyBoxesContext(ArrayPrototype.forEach));

// ES5 15.4.4.19
// http://es5.github.com/#x15.4.4.19
// https://developer.mozilla.org/en/Core_JavaScript_1.5_Reference/Objects/Array/map
defineProperties(ArrayPrototype, {
    map: function map(callbackfn/*, thisArg*/) {
        var object = ES.ToObject(this);
        var self = splitString && isString(this) ? strSplit(this, '') : object;
        var length = ES.ToUint32(self.length);
        var result = $Array(length);
        var T;
        if (arguments.length > 1) {
            T = arguments[1];
        }

        // If no callback function or if callback is not a callable function
        if (!isCallable(callbackfn)) {
            throw new TypeError('Array.prototype.map callback must be a function');
        }

        for (var i = 0; i < length; i++) {
            if (i in self) {
                if (typeof T === 'undefined') {
                    result[i] = callbackfn(self[i], i, object);
                } else {
                    result[i] = callbackfn.call(T, self[i], i, object);
                }
            }
        }
        return result;
    }
}, !properlyBoxesContext(ArrayPrototype.map));

// ES5 15.4.4.20
// http://es5.github.com/#x15.4.4.20
// https://developer.mozilla.org/en/Core_JavaScript_1.5_Reference/Objects/Array/filter
defineProperties(ArrayPrototype, {
    filter: function filter(callbackfn/*, thisArg*/) {
        var object = ES.ToObject(this);
        var self = splitString && isString(this) ? strSplit(this, '') : object;
        var length = ES.ToUint32(self.length);
        var result = [];
        var value;
        var T;
        if (arguments.length > 1) {
            T = arguments[1];
        }

        // If no callback function or if callback is not a callable function
        if (!isCallable(callbackfn)) {
            throw new TypeError('Array.prototype.filter callback must be a function');
        }

        for (var i = 0; i < length; i++) {
            if (i in self) {
                value = self[i];
                if (typeof T === 'undefined' ? callbackfn(value, i, object) : callbackfn.call(T, value, i, object)) {
                    pushCall(result, value);
                }
            }
        }
        return result;
    }
}, !properlyBoxesContext(ArrayPrototype.filter));

// ES5 15.4.4.16
// http://es5.github.com/#x15.4.4.16
// https://developer.mozilla.org/en/JavaScript/Reference/Global_Objects/Array/every
defineProperties(ArrayPrototype, {
    every: function every(callbackfn/*, thisArg*/) {
        var object = ES.ToObject(this);
        var self = splitString && isString(this) ? strSplit(this, '') : object;
        var length = ES.ToUint32(self.length);
        var T;
        if (arguments.length > 1) {
            T = arguments[1];
        }

        // If no callback function or if callback is not a callable function
        if (!isCallable(callbackfn)) {
            throw new TypeError('Array.prototype.every callback must be a function');
        }

        for (var i = 0; i < length; i++) {
            if (i in self && !(typeof T === 'undefined' ? callbackfn(self[i], i, object) : callbackfn.call(T, self[i], i, object))) {
                return false;
            }
        }
        return true;
    }
}, !properlyBoxesContext(ArrayPrototype.every));

// ES5 15.4.4.17
// http://es5.github.com/#x15.4.4.17
// https://developer.mozilla.org/en/JavaScript/Reference/Global_Objects/Array/some
defineProperties(ArrayPrototype, {
    some: function some(callbackfn/*, thisArg */) {
        var object = ES.ToObject(this);
        var self = splitString && isString(this) ? strSplit(this, '') : object;
        var length = ES.ToUint32(self.length);
        var T;
        if (arguments.length > 1) {
            T = arguments[1];
        }

        // If no callback function or if callback is not a callable function
        if (!isCallable(callbackfn)) {
            throw new TypeError('Array.prototype.some callback must be a function');
        }

        for (var i = 0; i < length; i++) {
            if (i in self && (typeof T === 'undefined' ? callbackfn(self[i], i, object) : callbackfn.call(T, self[i], i, object))) {
                return true;
            }
        }
        return false;
    }
}, !properlyBoxesContext(ArrayPrototype.some));

// ES5 15.4.4.21
// http://es5.github.com/#x15.4.4.21
// https://developer.mozilla.org/en/Core_JavaScript_1.5_Reference/Objects/Array/reduce
var reduceCoercesToObject = false;
if (ArrayPrototype.reduce) {
    reduceCoercesToObject = typeof ArrayPrototype.reduce.call('es5', function (_, __, ___, list) { return list; }) === 'object';
}
defineProperties(ArrayPrototype, {
    reduce: function reduce(callbackfn/*, initialValue*/) {
        var object = ES.ToObject(this);
        var self = splitString && isString(this) ? strSplit(this, '') : object;
        var length = ES.ToUint32(self.length);

        // If no callback function or if callback is not a callable function
        if (!isCallable(callbackfn)) {
            throw new TypeError('Array.prototype.reduce callback must be a function');
        }

        // no value to return if no initial value and an empty array
        if (length === 0 && arguments.length === 1) {
            throw new TypeError('reduce of empty array with no initial value');
        }

        var i = 0;
        var result;
        if (arguments.length >= 2) {
            result = arguments[1];
        } else {
            do {
                if (i in self) {
                    result = self[i++];
                    break;
                }

                // if array contains no values, no initial value to return
                if (++i >= length) {
                    throw new TypeError('reduce of empty array with no initial value');
                }
            } while (true);
        }

        for (; i < length; i++) {
            if (i in self) {
                result = callbackfn(result, self[i], i, object);
            }
        }

        return result;
    }
}, !reduceCoercesToObject);

// ES5 15.4.4.22
// http://es5.github.com/#x15.4.4.22
// https://developer.mozilla.org/en/Core_JavaScript_1.5_Reference/Objects/Array/reduceRight
var reduceRightCoercesToObject = false;
if (ArrayPrototype.reduceRight) {
    reduceRightCoercesToObject = typeof ArrayPrototype.reduceRight.call('es5', function (_, __, ___, list) { return list; }) === 'object';
}
defineProperties(ArrayPrototype, {
    reduceRight: function reduceRight(callbackfn/*, initial*/) {
        var object = ES.ToObject(this);
        var self = splitString && isString(this) ? strSplit(this, '') : object;
        var length = ES.ToUint32(self.length);

        // If no callback function or if callback is not a callable function
        if (!isCallable(callbackfn)) {
            throw new TypeError('Array.prototype.reduceRight callback must be a function');
        }

        // no value to return if no initial value, empty array
        if (length === 0 && arguments.length === 1) {
            throw new TypeError('reduceRight of empty array with no initial value');
        }

        var result;
        var i = length - 1;
        if (arguments.length >= 2) {
            result = arguments[1];
        } else {
            do {
                if (i in self) {
                    result = self[i--];
                    break;
                }

                // if array contains no values, no initial value to return
                if (--i < 0) {
                    throw new TypeError('reduceRight of empty array with no initial value');
                }
            } while (true);
        }

        if (i < 0) {
            return result;
        }

        do {
            if (i in self) {
                result = callbackfn(result, self[i], i, object);
            }
        } while (i--);

        return result;
    }
}, !reduceRightCoercesToObject);

// ES5 15.4.4.14
// http://es5.github.com/#x15.4.4.14
// https://developer.mozilla.org/en/JavaScript/Reference/Global_Objects/Array/indexOf
var hasFirefox2IndexOfBug = ArrayPrototype.indexOf && [0, 1].indexOf(1, 2) !== -1;
defineProperties(ArrayPrototype, {
    indexOf: function indexOf(searchElement/*, fromIndex */) {
        var self = splitString && isString(this) ? strSplit(this, '') : ES.ToObject(this);
        var length = ES.ToUint32(self.length);

        if (length === 0) {
            return -1;
        }

        var i = 0;
        if (arguments.length > 1) {
            i = ES.ToInteger(arguments[1]);
        }

        // handle negative indices
        i = i >= 0 ? i : max(0, length + i);
        for (; i < length; i++) {
            if (i in self && self[i] === searchElement) {
                return i;
            }
        }
        return -1;
    }
}, hasFirefox2IndexOfBug);

// ES5 15.4.4.15
// http://es5.github.com/#x15.4.4.15
// https://developer.mozilla.org/en/JavaScript/Reference/Global_Objects/Array/lastIndexOf
var hasFirefox2LastIndexOfBug = ArrayPrototype.lastIndexOf && [0, 1].lastIndexOf(0, -3) !== -1;
defineProperties(ArrayPrototype, {
    lastIndexOf: function lastIndexOf(searchElement/*, fromIndex */) {
        var self = splitString && isString(this) ? strSplit(this, '') : ES.ToObject(this);
        var length = ES.ToUint32(self.length);

        if (length === 0) {
            return -1;
        }
        var i = length - 1;
        if (arguments.length > 1) {
            i = min(i, ES.ToInteger(arguments[1]));
        }
        // handle negative indices
        i = i >= 0 ? i : length - Math.abs(i);
        for (; i >= 0; i--) {
            if (i in self && searchElement === self[i]) {
                return i;
            }
        }
        return -1;
    }
}, hasFirefox2LastIndexOfBug);

// ES5 15.4.4.12
// http://es5.github.com/#x15.4.4.12
var spliceNoopReturnsEmptyArray = (function () {
    var a = [1, 2];
    var result = a.splice();
    return a.length === 2 && isArray(result) && result.length === 0;
}());
defineProperties(ArrayPrototype, {
    // Safari 5.0 bug where .splice() returns undefined
    splice: function splice(start, deleteCount) {
        if (arguments.length === 0) {
            return [];
        } else {
            return array_splice.apply(this, arguments);
        }
    }
}, !spliceNoopReturnsEmptyArray);

var spliceWorksWithEmptyObject = (function () {
    var obj = {};
    ArrayPrototype.splice.call(obj, 0, 0, 1);
    return obj.length === 1;
}());
defineProperties(ArrayPrototype, {
    splice: function splice(start, deleteCount) {
        if (arguments.length === 0) { return []; }
        var args = arguments;
        this.length = max(ES.ToInteger(this.length), 0);
        if (arguments.length > 0 && typeof deleteCount !== 'number') {
            args = arraySlice(arguments);
            if (args.length < 2) {
                pushCall(args, this.length - start);
            } else {
                args[1] = ES.ToInteger(deleteCount);
            }
        }
        return array_splice.apply(this, args);
    }
}, !spliceWorksWithEmptyObject);
var spliceWorksWithLargeSparseArrays = (function () {
    // Per https://github.com/es-shims/es5-shim/issues/295
    // Safari 7/8 breaks with sparse arrays of size 1e5 or greater
    var arr = new $Array(1e5);
    // note: the index MUST be 8 or larger or the test will false pass
    arr[8] = 'x';
    arr.splice(1, 1);
    // note: this test must be defined *after* the indexOf shim
    // per https://github.com/es-shims/es5-shim/issues/313
    return arr.indexOf('x') === 7;
}());
var spliceWorksWithSmallSparseArrays = (function () {
    // Per https://github.com/es-shims/es5-shim/issues/295
    // Opera 12.15 breaks on this, no idea why.
    var n = 256;
    var arr = [];
    arr[n] = 'a';
    arr.splice(n + 1, 0, 'b');
    return arr[n] === 'a';
}());
defineProperties(ArrayPrototype, {
    splice: function splice(start, deleteCount) {
        var O = ES.ToObject(this);
        var A = [];
        var len = ES.ToUint32(O.length);
        var relativeStart = ES.ToInteger(start);
        var actualStart = relativeStart < 0 ? max((len + relativeStart), 0) : min(relativeStart, len);
        var actualDeleteCount = min(max(ES.ToInteger(deleteCount), 0), len - actualStart);

        var k = 0;
        var from;
        while (k < actualDeleteCount) {
            from = $String(actualStart + k);
            if (owns(O, from)) {
                A[k] = O[from];
            }
            k += 1;
        }

        var items = arraySlice(arguments, 2);
        var itemCount = items.length;
        var to;
        if (itemCount < actualDeleteCount) {
            k = actualStart;
            var maxK = len - actualDeleteCount;
            while (k < maxK) {
                from = $String(k + actualDeleteCount);
                to = $String(k + itemCount);
                if (owns(O, from)) {
                    O[to] = O[from];
                } else {
                    delete O[to];
                }
                k += 1;
            }
            k = len;
            var minK = len - actualDeleteCount + itemCount;
            while (k > minK) {
                delete O[k - 1];
                k -= 1;
            }
        } else if (itemCount > actualDeleteCount) {
            k = len - actualDeleteCount;
            while (k > actualStart) {
                from = $String(k + actualDeleteCount - 1);
                to = $String(k + itemCount - 1);
                if (owns(O, from)) {
                    O[to] = O[from];
                } else {
                    delete O[to];
                }
                k -= 1;
            }
        }
        k = actualStart;
        for (var i = 0; i < items.length; ++i) {
            O[k] = items[i];
            k += 1;
        }
        O.length = len - actualDeleteCount + itemCount;

        return A;
    }
}, !spliceWorksWithLargeSparseArrays || !spliceWorksWithSmallSparseArrays);

var originalJoin = ArrayPrototype.join;
var hasStringJoinBug;
try {
    hasStringJoinBug = Array.prototype.join.call('123', ',') !== '1,2,3';
} catch (e) {
    hasStringJoinBug = true;
}
if (hasStringJoinBug) {
    defineProperties(ArrayPrototype, {
        join: function join(separator) {
            var sep = typeof separator === 'undefined' ? ',' : separator;
            return originalJoin.call(isString(this) ? strSplit(this, '') : this, sep);
        }
    }, hasStringJoinBug);
}

var hasJoinUndefinedBug = [1, 2].join(undefined) !== '1,2';
if (hasJoinUndefinedBug) {
    defineProperties(ArrayPrototype, {
        join: function join(separator) {
            var sep = typeof separator === 'undefined' ? ',' : separator;
            return originalJoin.call(this, sep);
        }
    }, hasJoinUndefinedBug);
}

var pushShim = function push(item) {
    var O = ES.ToObject(this);
    var n = ES.ToUint32(O.length);
    var i = 0;
    while (i < arguments.length) {
        O[n + i] = arguments[i];
        i += 1;
    }
    O.length = n + i;
    return n + i;
};

var pushIsNotGeneric = (function () {
    var obj = {};
    var result = Array.prototype.push.call(obj, undefined);
    return result !== 1 || obj.length !== 1 || typeof obj[0] !== 'undefined' || !owns(obj, 0);
}());
defineProperties(ArrayPrototype, {
    push: function push(item) {
        if (isArray(this)) {
            return array_push.apply(this, arguments);
        }
        return pushShim.apply(this, arguments);
    }
}, pushIsNotGeneric);

// This fixes a very weird bug in Opera 10.6 when pushing `undefined
var pushUndefinedIsWeird = (function () {
    var arr = [];
    var result = arr.push(undefined);
    return result !== 1 || arr.length !== 1 || typeof arr[0] !== 'undefined' || !owns(arr, 0);
}());
defineProperties(ArrayPrototype, { push: pushShim }, pushUndefinedIsWeird);

// ES5 15.2.3.14
// http://es5.github.io/#x15.4.4.10
// Fix boxed string bug
defineProperties(ArrayPrototype, {
    slice: function (start, end) {
        var arr = isString(this) ? strSplit(this, '') : this;
        return arraySliceApply(arr, arguments);
    }
}, splitString);

var sortIgnoresNonFunctions = (function () {
    try {
        [1, 2].sort(null);
        [1, 2].sort({});
        return true;
    } catch (e) { /**/ }
    return false;
}());
var sortThrowsOnRegex = (function () {
    // this is a problem in Firefox 4, in which `typeof /a/ === 'function'`
    try {
        [1, 2].sort(/a/);
        return false;
    } catch (e) { /**/ }
    return true;
}());
var sortIgnoresUndefined = (function () {
    // applies in IE 8, for one.
    try {
        [1, 2].sort(undefined);
        return true;
    } catch (e) { /**/ }
    return false;
}());
defineProperties(ArrayPrototype, {
    sort: function sort(compareFn) {
        if (typeof compareFn === 'undefined') {
            return arraySort(this);
        }
        if (!isCallable(compareFn)) {
            throw new TypeError('Array.prototype.sort callback must be a function');
        }
        return arraySort(this, compareFn);
    }
}, sortIgnoresNonFunctions || !sortIgnoresUndefined || !sortThrowsOnRegex);

//
// Object
// ======
//

// ES5 15.2.3.14
// http://es5.github.com/#x15.2.3.14

// http://whattheheadsaid.com/2010/10/a-safer-object-keys-compatibility-implementation
var hasDontEnumBug = !({ 'toString': null }).propertyIsEnumerable('toString');
var hasProtoEnumBug = function () {}.propertyIsEnumerable('prototype');
var hasStringEnumBug = !owns('x', '0');
var equalsConstructorPrototype = function (o) {
    var ctor = o.constructor;
    return ctor && ctor.prototype === o;
};
var blacklistedKeys = {
    $window: true,
    $console: true,
    $parent: true,
    $self: true,
    $frame: true,
    $frames: true,
    $frameElement: true,
    $webkitIndexedDB: true,
    $webkitStorageInfo: true,
    $external: true
};
var hasAutomationEqualityBug = (function () {
    /* globals window */
    if (typeof window === 'undefined') { return false; }
    for (var k in window) {
        try {
            if (!blacklistedKeys['$' + k] && owns(window, k) && window[k] !== null && typeof window[k] === 'object') {
                equalsConstructorPrototype(window[k]);
            }
        } catch (e) {
            return true;
        }
    }
    return false;
}());
var equalsConstructorPrototypeIfNotBuggy = function (object) {
    if (typeof window === 'undefined' || !hasAutomationEqualityBug) { return equalsConstructorPrototype(object); }
    try {
        return equalsConstructorPrototype(object);
    } catch (e) {
        return false;
    }
};
var dontEnums = [
    'toString',
    'toLocaleString',
    'valueOf',
    'hasOwnProperty',
    'isPrototypeOf',
    'propertyIsEnumerable',
    'constructor'
];
var dontEnumsLength = dontEnums.length;

// taken directly from https://github.com/ljharb/is-arguments/blob/master/index.js
// can be replaced with require('is-arguments') if we ever use a build process instead
var isStandardArguments = function isArguments(value) {
    return toStr(value) === '[object Arguments]';
};
var isLegacyArguments = function isArguments(value) {
    return value !== null &&
        typeof value === 'object' &&
        typeof value.length === 'number' &&
        value.length >= 0 &&
        !isArray(value) &&
        isCallable(value.callee);
};
var isArguments = isStandardArguments(arguments) ? isStandardArguments : isLegacyArguments;

defineProperties($Object, {
    keys: function keys(object) {
        var isFn = isCallable(object);
        var isArgs = isArguments(object);
        var isObject = object !== null && typeof object === 'object';
        var isStr = isObject && isString(object);

        if (!isObject && !isFn && !isArgs) {
            throw new TypeError('Object.keys called on a non-object');
        }

        var theKeys = [];
        var skipProto = hasProtoEnumBug && isFn;
        if ((isStr && hasStringEnumBug) || isArgs) {
            for (var i = 0; i < object.length; ++i) {
                pushCall(theKeys, $String(i));
            }
        }

        if (!isArgs) {
            for (var name in object) {
                if (!(skipProto && name === 'prototype') && owns(object, name)) {
                    pushCall(theKeys, $String(name));
                }
            }
        }

        if (hasDontEnumBug) {
            var skipConstructor = equalsConstructorPrototypeIfNotBuggy(object);
            for (var j = 0; j < dontEnumsLength; j++) {
                var dontEnum = dontEnums[j];
                if (!(skipConstructor && dontEnum === 'constructor') && owns(object, dontEnum)) {
                    pushCall(theKeys, dontEnum);
                }
            }
        }
        return theKeys;
    }
});

var keysWorksWithArguments = $Object.keys && (function () {
    // Safari 5.0 bug
    return $Object.keys(arguments).length === 2;
}(1, 2));
var keysHasArgumentsLengthBug = $Object.keys && (function () {
    var argKeys = $Object.keys(arguments);
    return arguments.length !== 1 || argKeys.length !== 1 || argKeys[0] !== 1;
}(1));
var originalKeys = $Object.keys;
defineProperties($Object, {
    keys: function keys(object) {
        if (isArguments(object)) {
            return originalKeys(arraySlice(object));
        } else {
            return originalKeys(object);
        }
    }
}, !keysWorksWithArguments || keysHasArgumentsLengthBug);

//
// Date
// ====
//

var hasNegativeMonthYearBug = new Date(-3509827329600292).getUTCMonth() !== 0;
var aNegativeTestDate = new Date(-1509842289600292);
var aPositiveTestDate = new Date(1449662400000);
var hasToUTCStringFormatBug = aNegativeTestDate.toUTCString() !== 'Mon, 01 Jan -45875 11:59:59 GMT';
var hasToDateStringFormatBug;
var hasToStringFormatBug;
var timeZoneOffset = aNegativeTestDate.getTimezoneOffset();
if (timeZoneOffset < -720) {
    hasToDateStringFormatBug = aNegativeTestDate.toDateString() !== 'Tue Jan 02 -45875';
    hasToStringFormatBug = !(/^Thu Dec 10 2015 \d\d:\d\d:\d\d GMT[-\+]\d\d\d\d(?: |$)/).test(aPositiveTestDate.toString());
} else {
    hasToDateStringFormatBug = aNegativeTestDate.toDateString() !== 'Mon Jan 01 -45875';
    hasToStringFormatBug = !(/^Wed Dec 09 2015 \d\d:\d\d:\d\d GMT[-\+]\d\d\d\d(?: |$)/).test(aPositiveTestDate.toString());
}

var originalGetFullYear = call.bind(Date.prototype.getFullYear);
var originalGetMonth = call.bind(Date.prototype.getMonth);
var originalGetDate = call.bind(Date.prototype.getDate);
var originalGetUTCFullYear = call.bind(Date.prototype.getUTCFullYear);
var originalGetUTCMonth = call.bind(Date.prototype.getUTCMonth);
var originalGetUTCDate = call.bind(Date.prototype.getUTCDate);
var originalGetUTCDay = call.bind(Date.prototype.getUTCDay);
var originalGetUTCHours = call.bind(Date.prototype.getUTCHours);
var originalGetUTCMinutes = call.bind(Date.prototype.getUTCMinutes);
var originalGetUTCSeconds = call.bind(Date.prototype.getUTCSeconds);
var originalGetUTCMilliseconds = call.bind(Date.prototype.getUTCMilliseconds);
var dayName = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];
var monthName = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'];
var daysInMonth = function daysInMonth(month, year) {
    return originalGetDate(new Date(year, month, 0));
};

defineProperties(Date.prototype, {
    getFullYear: function getFullYear() {
        if (!this || !(this instanceof Date)) {
            throw new TypeError('this is not a Date object.');
        }
        var year = originalGetFullYear(this);
        if (year < 0 && originalGetMonth(this) > 11) {
            return year + 1;
        }
        return year;
    },
    getMonth: function getMonth() {
        if (!this || !(this instanceof Date)) {
            throw new TypeError('this is not a Date object.');
        }
        var year = originalGetFullYear(this);
        var month = originalGetMonth(this);
        if (year < 0 && month > 11) {
            return 0;
        }
        return month;
    },
    getDate: function getDate() {
        if (!this || !(this instanceof Date)) {
            throw new TypeError('this is not a Date object.');
        }
        var year = originalGetFullYear(this);
        var month = originalGetMonth(this);
        var date = originalGetDate(this);
        if (year < 0 && month > 11) {
            if (month === 12) {
                return date;
            }
            var days = daysInMonth(0, year + 1);
            return (days - date) + 1;
        }
        return date;
    },
    getUTCFullYear: function getUTCFullYear() {
        if (!this || !(this instanceof Date)) {
            throw new TypeError('this is not a Date object.');
        }
        var year = originalGetUTCFullYear(this);
        if (year < 0 && originalGetUTCMonth(this) > 11) {
            return year + 1;
        }
        return year;
    },
    getUTCMonth: function getUTCMonth() {
        if (!this || !(this instanceof Date)) {
            throw new TypeError('this is not a Date object.');
        }
        var year = originalGetUTCFullYear(this);
        var month = originalGetUTCMonth(this);
        if (year < 0 && month > 11) {
            return 0;
        }
        return month;
    },
    getUTCDate: function getUTCDate() {
        if (!this || !(this instanceof Date)) {
            throw new TypeError('this is not a Date object.');
        }
        var year = originalGetUTCFullYear(this);
        var month = originalGetUTCMonth(this);
        var date = originalGetUTCDate(this);
        if (year < 0 && month > 11) {
            if (month === 12) {
                return date;
            }
            var days = daysInMonth(0, year + 1);
            return (days - date) + 1;
        }
        return date;
    }
}, hasNegativeMonthYearBug);

defineProperties(Date.prototype, {
    toUTCString: function toUTCString() {
        if (!this || !(this instanceof Date)) {
            throw new TypeError('this is not a Date object.');
        }
        var day = originalGetUTCDay(this);
        var date = originalGetUTCDate(this);
        var month = originalGetUTCMonth(this);
        var year = originalGetUTCFullYear(this);
        var hour = originalGetUTCHours(this);
        var minute = originalGetUTCMinutes(this);
        var second = originalGetUTCSeconds(this);
        return dayName[day] + ', ' +
            (date < 10 ? '0' + date : date) + ' ' +
            monthName[month] + ' ' +
            year + ' ' +
            (hour < 10 ? '0' + hour : hour) + ':' +
            (minute < 10 ? '0' + minute : minute) + ':' +
            (second < 10 ? '0' + second : second) + ' GMT';
    }
}, hasNegativeMonthYearBug || hasToUTCStringFormatBug);

// Opera 12 has `,`
defineProperties(Date.prototype, {
    toDateString: function toDateString() {
        if (!this || !(this instanceof Date)) {
            throw new TypeError('this is not a Date object.');
        }
        var day = this.getDay();
        var date = this.getDate();
        var month = this.getMonth();
        var year = this.getFullYear();
        return dayName[day] + ' ' +
            monthName[month] + ' ' +
            (date < 10 ? '0' + date : date) + ' ' +
            year;
    }
}, hasNegativeMonthYearBug || hasToDateStringFormatBug);

// can't use defineProperties here because of toString enumeration issue in IE <= 8
if (hasNegativeMonthYearBug || hasToStringFormatBug) {
    Date.prototype.toString = function toString() {
        if (!this || !(this instanceof Date)) {
            throw new TypeError('this is not a Date object.');
        }
        var day = this.getDay();
        var date = this.getDate();
        var month = this.getMonth();
        var year = this.getFullYear();
        var hour = this.getHours();
        var minute = this.getMinutes();
        var second = this.getSeconds();
        var timezoneOffset = this.getTimezoneOffset();
        var hoursOffset = Math.floor(Math.abs(timezoneOffset) / 60);
        var minutesOffset = Math.floor(Math.abs(timezoneOffset) % 60);
        return dayName[day] + ' ' +
            monthName[month] + ' ' +
            (date < 10 ? '0' + date : date) + ' ' +
            year + ' ' +
            (hour < 10 ? '0' + hour : hour) + ':' +
            (minute < 10 ? '0' + minute : minute) + ':' +
            (second < 10 ? '0' + second : second) + ' GMT' +
            (timezoneOffset > 0 ? '-' : '+') +
            (hoursOffset < 10 ? '0' + hoursOffset : hoursOffset) +
            (minutesOffset < 10 ? '0' + minutesOffset : minutesOffset);
    };
    if (supportsDescriptors) {
        $Object.defineProperty(Date.prototype, 'toString', {
            configurable: true,
            enumerable: false,
            writable: true
        });
    }
}

// ES5 15.9.5.43
// http://es5.github.com/#x15.9.5.43
// This function returns a String value represent the instance in time
// represented by this Date object. The format of the String is the Date Time
// string format defined in 15.9.1.15. All fields are present in the String.
// The time zone is always UTC, denoted by the suffix Z. If the time value of
// this object is not a finite Number a RangeError exception is thrown.
var negativeDate = -62198755200000;
var negativeYearString = '-000001';
var hasNegativeDateBug = Date.prototype.toISOString && new Date(negativeDate).toISOString().indexOf(negativeYearString) === -1;
var hasSafari51DateBug = Date.prototype.toISOString && new Date(-1).toISOString() !== '1969-12-31T23:59:59.999Z';

var getTime = call.bind(Date.prototype.getTime);

defineProperties(Date.prototype, {
    toISOString: function toISOString() {
        if (!isFinite(this) || !isFinite(getTime(this))) {
            // Adope Photoshop requires the second check.
            throw new RangeError('Date.prototype.toISOString called on non-finite value.');
        }

        var year = originalGetUTCFullYear(this);

        var month = originalGetUTCMonth(this);
        // see https://github.com/es-shims/es5-shim/issues/111
        year += Math.floor(month / 12);
        month = (month % 12 + 12) % 12;

        // the date time string format is specified in 15.9.1.15.
        var result = [month + 1, originalGetUTCDate(this), originalGetUTCHours(this), originalGetUTCMinutes(this), originalGetUTCSeconds(this)];
        year = (
            (year < 0 ? '-' : (year > 9999 ? '+' : '')) +
            strSlice('00000' + Math.abs(year), (0 <= year && year <= 9999) ? -4 : -6)
        );

        for (var i = 0; i < result.length; ++i) {
          // pad months, days, hours, minutes, and seconds to have two digits.
          result[i] = strSlice('00' + result[i], -2);
        }
        // pad milliseconds to have three digits.
        return (
            year + '-' + arraySlice(result, 0, 2).join('-') +
            'T' + arraySlice(result, 2).join(':') + '.' +
            strSlice('000' + originalGetUTCMilliseconds(this), -3) + 'Z'
        );
    }
}, hasNegativeDateBug || hasSafari51DateBug);

// ES5 15.9.5.44
// http://es5.github.com/#x15.9.5.44
// This function provides a String representation of a Date object for use by
// JSON.stringify (15.12.3).
var dateToJSONIsSupported = (function () {
    try {
        return Date.prototype.toJSON &&
            new Date(NaN).toJSON() === null &&
            new Date(negativeDate).toJSON().indexOf(negativeYearString) !== -1 &&
            Date.prototype.toJSON.call({ // generic
                toISOString: function () { return true; }
            });
    } catch (e) {
        return false;
    }
}());
if (!dateToJSONIsSupported) {
    Date.prototype.toJSON = function toJSON(key) {
        // When the toJSON method is called with argument key, the following
        // steps are taken:

        // 1.  Let O be the result of calling ToObject, giving it the this
        // value as its argument.
        // 2. Let tv be ES.ToPrimitive(O, hint Number).
        var O = $Object(this);
        var tv = ES.ToPrimitive(O);
        // 3. If tv is a Number and is not finite, return null.
        if (typeof tv === 'number' && !isFinite(tv)) {
            return null;
        }
        // 4. Let toISO be the result of calling the [[Get]] internal method of
        // O with argument "toISOString".
        var toISO = O.toISOString;
        // 5. If IsCallable(toISO) is false, throw a TypeError exception.
        if (!isCallable(toISO)) {
            throw new TypeError('toISOString property is not callable');
        }
        // 6. Return the result of calling the [[Call]] internal method of
        //  toISO with O as the this value and an empty argument list.
        return toISO.call(O);

        // NOTE 1 The argument is ignored.

        // NOTE 2 The toJSON function is intentionally generic; it does not
        // require that its this value be a Date object. Therefore, it can be
        // transferred to other kinds of objects for use as a method. However,
        // it does require that any such object have a toISOString method. An
        // object is free to use the argument key to filter its
        // stringification.
    };
}

// ES5 15.9.4.2
// http://es5.github.com/#x15.9.4.2
// based on work shared by Daniel Friesen (dantman)
// http://gist.github.com/303249
var supportsExtendedYears = Date.parse('+033658-09-27T01:46:40.000Z') === 1e15;
var acceptsInvalidDates = !isNaN(Date.parse('2012-04-04T24:00:00.500Z')) || !isNaN(Date.parse('2012-11-31T23:59:59.000Z')) || !isNaN(Date.parse('2012-12-31T23:59:60.000Z'));
var doesNotParseY2KNewYear = isNaN(Date.parse('2000-01-01T00:00:00.000Z'));
if (doesNotParseY2KNewYear || acceptsInvalidDates || !supportsExtendedYears) {
    // XXX global assignment won't work in embeddings that use
    // an alternate object for the context.
    /* global Date: true */
    /* eslint-disable no-undef */
    var maxSafeUnsigned32Bit = Math.pow(2, 31) - 1;
    var hasSafariSignedIntBug = isActualNaN(new Date(1970, 0, 1, 0, 0, 0, maxSafeUnsigned32Bit + 1).getTime());
    /* eslint-disable no-implicit-globals */
    Date = (function (NativeDate) {
    /* eslint-enable no-implicit-globals */
    /* eslint-enable no-undef */
        // Date.length === 7
        var DateShim = function Date(Y, M, D, h, m, s, ms) {
            var length = arguments.length;
            var date;
            if (this instanceof NativeDate) {
                var seconds = s;
                var millis = ms;
                if (hasSafariSignedIntBug && length >= 7 && ms > maxSafeUnsigned32Bit) {
                    // work around a Safari 8/9 bug where it treats the seconds as signed
                    var msToShift = Math.floor(ms / maxSafeUnsigned32Bit) * maxSafeUnsigned32Bit;
                    var sToShift = Math.floor(msToShift / 1e3);
                    seconds += sToShift;
                    millis -= sToShift * 1e3;
                }
                date = length === 1 && $String(Y) === Y ? // isString(Y)
                    // We explicitly pass it through parse:
                    new NativeDate(DateShim.parse(Y)) :
                    // We have to manually make calls depending on argument
                    // length here
                    length >= 7 ? new NativeDate(Y, M, D, h, m, seconds, millis) :
                    length >= 6 ? new NativeDate(Y, M, D, h, m, seconds) :
                    length >= 5 ? new NativeDate(Y, M, D, h, m) :
                    length >= 4 ? new NativeDate(Y, M, D, h) :
                    length >= 3 ? new NativeDate(Y, M, D) :
                    length >= 2 ? new NativeDate(Y, M) :
                    length >= 1 ? new NativeDate(Y instanceof NativeDate ? +Y : Y) :
                                  new NativeDate();
            } else {
                date = NativeDate.apply(this, arguments);
            }
            if (!isPrimitive(date)) {
              // Prevent mixups with unfixed Date object
              defineProperties(date, { constructor: DateShim }, true);
            }
            return date;
        };

        // 15.9.1.15 Date Time String Format.
        var isoDateExpression = new RegExp('^' +
            '(\\d{4}|[+-]\\d{6})' + // four-digit year capture or sign +
                                      // 6-digit extended year
            '(?:-(\\d{2})' + // optional month capture
            '(?:-(\\d{2})' + // optional day capture
            '(?:' + // capture hours:minutes:seconds.milliseconds
                'T(\\d{2})' + // hours capture
                ':(\\d{2})' + // minutes capture
                '(?:' + // optional :seconds.milliseconds
                    ':(\\d{2})' + // seconds capture
                    '(?:(\\.\\d{1,}))?' + // milliseconds capture
                ')?' +
            '(' + // capture UTC offset component
                'Z|' + // UTC capture
                '(?:' + // offset specifier +/-hours:minutes
                    '([-+])' + // sign capture
                    '(\\d{2})' + // hours offset capture
                    ':(\\d{2})' + // minutes offset capture
                ')' +
            ')?)?)?)?' +
        '$');

        var months = [0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365];

        var dayFromMonth = function dayFromMonth(year, month) {
            var t = month > 1 ? 1 : 0;
            return (
                months[month] +
                Math.floor((year - 1969 + t) / 4) -
                Math.floor((year - 1901 + t) / 100) +
                Math.floor((year - 1601 + t) / 400) +
                365 * (year - 1970)
            );
        };

        var toUTC = function toUTC(t) {
            var s = 0;
            var ms = t;
            if (hasSafariSignedIntBug && ms > maxSafeUnsigned32Bit) {
                // work around a Safari 8/9 bug where it treats the seconds as signed
                var msToShift = Math.floor(ms / maxSafeUnsigned32Bit) * maxSafeUnsigned32Bit;
                var sToShift = Math.floor(msToShift / 1e3);
                s += sToShift;
                ms -= sToShift * 1e3;
            }
            return $Number(new NativeDate(1970, 0, 1, 0, 0, s, ms));
        };

        // Copy any custom methods a 3rd party library may have added
        for (var key in NativeDate) {
            if (owns(NativeDate, key)) {
                DateShim[key] = NativeDate[key];
            }
        }

        // Copy "native" methods explicitly; they may be non-enumerable
        defineProperties(DateShim, {
            now: NativeDate.now,
            UTC: NativeDate.UTC
        }, true);
        DateShim.prototype = NativeDate.prototype;
        defineProperties(DateShim.prototype, {
            constructor: DateShim
        }, true);

        // Upgrade Date.parse to handle simplified ISO 8601 strings
        var parseShim = function parse(string) {
            var match = isoDateExpression.exec(string);
            if (match) {
                // parse months, days, hours, minutes, seconds, and milliseconds
                // provide default values if necessary
                // parse the UTC offset component
                var year = $Number(match[1]),
                    month = $Number(match[2] || 1) - 1,
                    day = $Number(match[3] || 1) - 1,
                    hour = $Number(match[4] || 0),
                    minute = $Number(match[5] || 0),
                    second = $Number(match[6] || 0),
                    millisecond = Math.floor($Number(match[7] || 0) * 1000),
                    // When time zone is missed, local offset should be used
                    // (ES 5.1 bug)
                    // see https://bugs.ecmascript.org/show_bug.cgi?id=112
                    isLocalTime = Boolean(match[4] && !match[8]),
                    signOffset = match[9] === '-' ? 1 : -1,
                    hourOffset = $Number(match[10] || 0),
                    minuteOffset = $Number(match[11] || 0),
                    result;
                var hasMinutesOrSecondsOrMilliseconds = minute > 0 || second > 0 || millisecond > 0;
                if (
                    hour < (hasMinutesOrSecondsOrMilliseconds ? 24 : 25) &&
                    minute < 60 && second < 60 && millisecond < 1000 &&
                    month > -1 && month < 12 && hourOffset < 24 &&
                    minuteOffset < 60 && // detect invalid offsets
                    day > -1 &&
                    day < (dayFromMonth(year, month + 1) - dayFromMonth(year, month))
                ) {
                    result = (
                        (dayFromMonth(year, month) + day) * 24 +
                        hour +
                        hourOffset * signOffset
                    ) * 60;
                    result = (
                        (result + minute + minuteOffset * signOffset) * 60 +
                        second
                    ) * 1000 + millisecond;
                    if (isLocalTime) {
                        result = toUTC(result);
                    }
                    if (-8.64e15 <= result && result <= 8.64e15) {
                        return result;
                    }
                }
                return NaN;
            }
            return NativeDate.parse.apply(this, arguments);
        };
        defineProperties(DateShim, { parse: parseShim });

        return DateShim;
    }(Date));
    /* global Date: false */
}

// ES5 15.9.4.4
// http://es5.github.com/#x15.9.4.4
if (!Date.now) {
    Date.now = function now() {
        return new Date().getTime();
    };
}

//
// Number
// ======
//

// ES5.1 15.7.4.5
// http://es5.github.com/#x15.7.4.5
var hasToFixedBugs = NumberPrototype.toFixed && (
  (0.00008).toFixed(3) !== '0.000' ||
  (0.9).toFixed(0) !== '1' ||
  (1.255).toFixed(2) !== '1.25' ||
  (1000000000000000128).toFixed(0) !== '1000000000000000128'
);

var toFixedHelpers = {
  base: 1e7,
  size: 6,
  data: [0, 0, 0, 0, 0, 0],
  multiply: function multiply(n, c) {
      var i = -1;
      var c2 = c;
      while (++i < toFixedHelpers.size) {
          c2 += n * toFixedHelpers.data[i];
          toFixedHelpers.data[i] = c2 % toFixedHelpers.base;
          c2 = Math.floor(c2 / toFixedHelpers.base);
      }
  },
  divide: function divide(n) {
      var i = toFixedHelpers.size;
      var c = 0;
      while (--i >= 0) {
          c += toFixedHelpers.data[i];
          toFixedHelpers.data[i] = Math.floor(c / n);
          c = (c % n) * toFixedHelpers.base;
      }
  },
  numToString: function numToString() {
      var i = toFixedHelpers.size;
      var s = '';
      while (--i >= 0) {
          if (s !== '' || i === 0 || toFixedHelpers.data[i] !== 0) {
              var t = $String(toFixedHelpers.data[i]);
              if (s === '') {
                  s = t;
              } else {
                  s += strSlice('0000000', 0, 7 - t.length) + t;
              }
          }
      }
      return s;
  },
  pow: function pow(x, n, acc) {
      return (n === 0 ? acc : (n % 2 === 1 ? pow(x, n - 1, acc * x) : pow(x * x, n / 2, acc)));
  },
  log: function log(x) {
      var n = 0;
      var x2 = x;
      while (x2 >= 4096) {
          n += 12;
          x2 /= 4096;
      }
      while (x2 >= 2) {
          n += 1;
          x2 /= 2;
      }
      return n;
  }
};

var toFixedShim = function toFixed(fractionDigits) {
    var f, x, s, m, e, z, j, k;

    // Test for NaN and round fractionDigits down
    f = $Number(fractionDigits);
    f = isActualNaN(f) ? 0 : Math.floor(f);

    if (f < 0 || f > 20) {
        throw new RangeError('Number.toFixed called with invalid number of decimals');
    }

    x = $Number(this);

    if (isActualNaN(x)) {
        return 'NaN';
    }

    // If it is too big or small, return the string value of the number
    if (x <= -1e21 || x >= 1e21) {
        return $String(x);
    }

    s = '';

    if (x < 0) {
        s = '-';
        x = -x;
    }

    m = '0';

    if (x > 1e-21) {
        // 1e-21 < x < 1e21
        // -70 < log2(x) < 70
        e = toFixedHelpers.log(x * toFixedHelpers.pow(2, 69, 1)) - 69;
        z = (e < 0 ? x * toFixedHelpers.pow(2, -e, 1) : x / toFixedHelpers.pow(2, e, 1));
        z *= 0x10000000000000; // Math.pow(2, 52);
        e = 52 - e;

        // -18 < e < 122
        // x = z / 2 ^ e
        if (e > 0) {
            toFixedHelpers.multiply(0, z);
            j = f;

            while (j >= 7) {
                toFixedHelpers.multiply(1e7, 0);
                j -= 7;
            }

            toFixedHelpers.multiply(toFixedHelpers.pow(10, j, 1), 0);
            j = e - 1;

            while (j >= 23) {
                toFixedHelpers.divide(1 << 23);
                j -= 23;
            }

            toFixedHelpers.divide(1 << j);
            toFixedHelpers.multiply(1, 1);
            toFixedHelpers.divide(2);
            m = toFixedHelpers.numToString();
        } else {
            toFixedHelpers.multiply(0, z);
            toFixedHelpers.multiply(1 << (-e), 0);
            m = toFixedHelpers.numToString() + strSlice('0.00000000000000000000', 2, 2 + f);
        }
    }

    if (f > 0) {
        k = m.length;

        if (k <= f) {
            m = s + strSlice('0.0000000000000000000', 0, f - k + 2) + m;
        } else {
            m = s + strSlice(m, 0, k - f) + '.' + strSlice(m, k - f);
        }
    } else {
        m = s + m;
    }

    return m;
};
defineProperties(NumberPrototype, { toFixed: toFixedShim }, hasToFixedBugs);

var hasToPrecisionUndefinedBug = (function () {
    try {
        return 1.0.toPrecision(undefined) === '1';
    } catch (e) {
        return true;
    }
}());
var originalToPrecision = NumberPrototype.toPrecision;
defineProperties(NumberPrototype, {
    toPrecision: function toPrecision(precision) {
        return typeof precision === 'undefined' ? originalToPrecision.call(this) : originalToPrecision.call(this, precision);
    }
}, hasToPrecisionUndefinedBug);

//
// String
// ======
//

// ES5 15.5.4.14
// http://es5.github.com/#x15.5.4.14

// [bugfix, IE lt 9, firefox 4, Konqueror, Opera, obscure browsers]
// Many browsers do not split properly with regular expressions or they
// do not perform the split correctly under obscure conditions.
// See http://blog.stevenlevithan.com/archives/cross-browser-split
// I've tested in many browsers and this seems to cover the deviant ones:
//    'ab'.split(/(?:ab)*/) should be ["", ""], not [""]
//    '.'.split(/(.?)(.?)/) should be ["", ".", "", ""], not ["", ""]
//    'tesst'.split(/(s)*/) should be ["t", undefined, "e", "s", "t"], not
//       [undefined, "t", undefined, "e", ...]
//    ''.split(/.?/) should be [], not [""]
//    '.'.split(/()()/) should be ["."], not ["", "", "."]

if (
    'ab'.split(/(?:ab)*/).length !== 2 ||
    '.'.split(/(.?)(.?)/).length !== 4 ||
    'tesst'.split(/(s)*/)[1] === 't' ||
    'test'.split(/(?:)/, -1).length !== 4 ||
    ''.split(/.?/).length ||
    '.'.split(/()()/).length > 1
) {
    (function () {
        var compliantExecNpcg = typeof (/()??/).exec('')[1] === 'undefined'; // NPCG: nonparticipating capturing group
        var maxSafe32BitInt = Math.pow(2, 32) - 1;

        StringPrototype.split = function (separator, limit) {
            var string = String(this);
            if (typeof separator === 'undefined' && limit === 0) {
                return [];
            }

            // If `separator` is not a regex, use native split
            if (!isRegex(separator)) {
                return strSplit(this, separator, limit);
            }

            var output = [];
            var flags = (separator.ignoreCase ? 'i' : '') +
                        (separator.multiline ? 'm' : '') +
                        (separator.unicode ? 'u' : '') + // in ES6
                        (separator.sticky ? 'y' : ''), // Firefox 3+ and ES6
                lastLastIndex = 0,
                // Make `global` and avoid `lastIndex` issues by working with a copy
                separator2, match, lastIndex, lastLength;
            var separatorCopy = new RegExp(separator.source, flags + 'g');
            if (!compliantExecNpcg) {
                // Doesn't need flags gy, but they don't hurt
                separator2 = new RegExp('^' + separatorCopy.source + '$(?!\\s)', flags);
            }
            /* Values for `limit`, per the spec:
             * If undefined: 4294967295 // maxSafe32BitInt
             * If 0, Infinity, or NaN: 0
             * If positive number: limit = Math.floor(limit); if (limit > 4294967295) limit -= 4294967296;
             * If negative number: 4294967296 - Math.floor(Math.abs(limit))
             * If other: Type-convert, then use the above rules
             */
            var splitLimit = typeof limit === 'undefined' ? maxSafe32BitInt : ES.ToUint32(limit);
            match = separatorCopy.exec(string);
            while (match) {
                // `separatorCopy.lastIndex` is not reliable cross-browser
                lastIndex = match.index + match[0].length;
                if (lastIndex > lastLastIndex) {
                    pushCall(output, strSlice(string, lastLastIndex, match.index));
                    // Fix browsers whose `exec` methods don't consistently return `undefined` for
                    // nonparticipating capturing groups
                    if (!compliantExecNpcg && match.length > 1) {
                        /* eslint-disable no-loop-func */
                        match[0].replace(separator2, function () {
                            for (var i = 1; i < arguments.length - 2; i++) {
                                if (typeof arguments[i] === 'undefined') {
                                    match[i] = void 0;
                                }
                            }
                        });
                        /* eslint-enable no-loop-func */
                    }
                    if (match.length > 1 && match.index < string.length) {
                        array_push.apply(output, arraySlice(match, 1));
                    }
                    lastLength = match[0].length;
                    lastLastIndex = lastIndex;
                    if (output.length >= splitLimit) {
                        break;
                    }
                }
                if (separatorCopy.lastIndex === match.index) {
                    separatorCopy.lastIndex++; // Avoid an infinite loop
                }
                match = separatorCopy.exec(string);
            }
            if (lastLastIndex === string.length) {
                if (lastLength || !separatorCopy.test('')) {
                    pushCall(output, '');
                }
            } else {
                pushCall(output, strSlice(string, lastLastIndex));
            }
            return output.length > splitLimit ? arraySlice(output, 0, splitLimit) : output;
        };
    }());

// [bugfix, chrome]
// If separator is undefined, then the result array contains just one String,
// which is the this value (converted to a String). If limit is not undefined,
// then the output array is truncated so that it contains no more than limit
// elements.
// "0".split(undefined, 0) -> []
} else if ('0'.split(void 0, 0).length) {
    StringPrototype.split = function split(separator, limit) {
        if (typeof separator === 'undefined' && limit === 0) { return []; }
        return strSplit(this, separator, limit);
    };
}

var str_replace = StringPrototype.replace;
var replaceReportsGroupsCorrectly = (function () {
    var groups = [];
    'x'.replace(/x(.)?/g, function (match, group) {
        pushCall(groups, group);
    });
    return groups.length === 1 && typeof groups[0] === 'undefined';
}());

if (!replaceReportsGroupsCorrectly) {
    StringPrototype.replace = function replace(searchValue, replaceValue) {
        var isFn = isCallable(replaceValue);
        var hasCapturingGroups = isRegex(searchValue) && (/\)[*?]/).test(searchValue.source);
        if (!isFn || !hasCapturingGroups) {
            return str_replace.call(this, searchValue, replaceValue);
        } else {
            var wrappedReplaceValue = function (match) {
                var length = arguments.length;
                var originalLastIndex = searchValue.lastIndex;
                searchValue.lastIndex = 0;
                var args = searchValue.exec(match) || [];
                searchValue.lastIndex = originalLastIndex;
                pushCall(args, arguments[length - 2], arguments[length - 1]);
                return replaceValue.apply(this, args);
            };
            return str_replace.call(this, searchValue, wrappedReplaceValue);
        }
    };
}

// ECMA-262, 3rd B.2.3
// Not an ECMAScript standard, although ECMAScript 3rd Edition has a
// non-normative section suggesting uniform semantics and it should be
// normalized across all browsers
// [bugfix, IE lt 9] IE < 9 substr() with negative value not working in IE
var string_substr = StringPrototype.substr;
var hasNegativeSubstrBug = ''.substr && '0b'.substr(-1) !== 'b';
defineProperties(StringPrototype, {
    substr: function substr(start, length) {
        var normalizedStart = start;
        if (start < 0) {
            normalizedStart = max(this.length + start, 0);
        }
        return string_substr.call(this, normalizedStart, length);
    }
}, hasNegativeSubstrBug);

// ES5 15.5.4.20
// whitespace from: http://es5.github.io/#x15.5.4.20
var ws = '\x09\x0A\x0B\x0C\x0D\x20\xA0\u1680\u180E\u2000\u2001\u2002\u2003' +
    '\u2004\u2005\u2006\u2007\u2008\u2009\u200A\u202F\u205F\u3000\u2028' +
    '\u2029\uFEFF';
var zeroWidth = '\u200b';
var wsRegexChars = '[' + ws + ']';
var trimBeginRegexp = new RegExp('^' + wsRegexChars + wsRegexChars + '*');
var trimEndRegexp = new RegExp(wsRegexChars + wsRegexChars + '*$');
var hasTrimWhitespaceBug = StringPrototype.trim && (ws.trim() || !zeroWidth.trim());
defineProperties(StringPrototype, {
    // http://blog.stevenlevithan.com/archives/faster-trim-javascript
    // http://perfectionkills.com/whitespace-deviations/
    trim: function trim() {
        if (typeof this === 'undefined' || this === null) {
            throw new TypeError("can't convert " + this + ' to object');
        }
        return $String(this).replace(trimBeginRegexp, '').replace(trimEndRegexp, '');
    }
}, hasTrimWhitespaceBug);
var trim = call.bind(String.prototype.trim);

var hasLastIndexBug = StringPrototype.lastIndexOf && 'abcあい'.lastIndexOf('あい', 2) !== -1;
defineProperties(StringPrototype, {
    lastIndexOf: function lastIndexOf(searchString) {
        if (typeof this === 'undefined' || this === null) {
            throw new TypeError("can't convert " + this + ' to object');
        }
        var S = $String(this);
        var searchStr = $String(searchString);
        var numPos = arguments.length > 1 ? $Number(arguments[1]) : NaN;
        var pos = isActualNaN(numPos) ? Infinity : ES.ToInteger(numPos);
        var start = min(max(pos, 0), S.length);
        var searchLen = searchStr.length;
        var k = start + searchLen;
        while (k > 0) {
            k = max(0, k - searchLen);
            var index = strIndexOf(strSlice(S, k, start + searchLen), searchStr);
            if (index !== -1) {
                return k + index;
            }
        }
        return -1;
    }
}, hasLastIndexBug);

var originalLastIndexOf = StringPrototype.lastIndexOf;
defineProperties(StringPrototype, {
    lastIndexOf: function lastIndexOf(searchString) {
        return originalLastIndexOf.apply(this, arguments);
    }
}, StringPrototype.lastIndexOf.length !== 1);

// ES-5 15.1.2.2
/* eslint-disable radix */
if (parseInt(ws + '08') !== 8 || parseInt(ws + '0x16') !== 22) {
/* eslint-enable radix */
    /* global parseInt: true */
    parseInt = (function (origParseInt) {
        var hexRegex = /^[\-+]?0[xX]/;
        return function parseInt(str, radix) {
            var string = trim(str);
            var defaultedRadix = $Number(radix) || (hexRegex.test(string) ? 16 : 10);
            return origParseInt(string, defaultedRadix);
        };
    }(parseInt));
}

// https://es5.github.io/#x15.1.2.3
if (1 / parseFloat('-0') !== -Infinity) {
    /* global parseFloat: true */
    parseFloat = (function (origParseFloat) {
        return function parseFloat(string) {
            var inputString = trim(string);
            var result = origParseFloat(inputString);
            return result === 0 && strSlice(inputString, 0, 1) === '-' ? -0 : result;
        };
    }(parseFloat));
}

if (String(new RangeError('test')) !== 'RangeError: test') {
    var errorToStringShim = function toString() {
        if (typeof this === 'undefined' || this === null) {
            throw new TypeError("can't convert " + this + ' to object');
        }
        var name = this.name;
        if (typeof name === 'undefined') {
            name = 'Error';
        } else if (typeof name !== 'string') {
            name = $String(name);
        }
        var msg = this.message;
        if (typeof msg === 'undefined') {
            msg = '';
        } else if (typeof msg !== 'string') {
            msg = $String(msg);
        }
        if (!name) {
            return msg;
        }
        if (!msg) {
            return name;
        }
        return name + ': ' + msg;
    };
    // can't use defineProperties here because of toString enumeration issue in IE <= 8
    Error.prototype.toString = errorToStringShim;
}

if (supportsDescriptors) {
    var ensureNonEnumerable = function (obj, prop) {
        if (isEnum(obj, prop)) {
            var desc = Object.getOwnPropertyDescriptor(obj, prop);
            desc.enumerable = false;
            Object.defineProperty(obj, prop, desc);
        }
    };
    ensureNonEnumerable(Error.prototype, 'message');
    if (Error.prototype.message !== '') {
      Error.prototype.message = '';
    }
    ensureNonEnumerable(Error.prototype, 'name');
}

if (String(/a/mig) !== '/a/gim') {
    var regexToString = function toString() {
        var str = '/' + this.source + '/';
        if (this.global) {
            str += 'g';
        }
        if (this.ignoreCase) {
            str += 'i';
        }
        if (this.multiline) {
            str += 'm';
        }
        return str;
    };
    // can't use defineProperties here because of toString enumeration issue in IE <= 8
    RegExp.prototype.toString = regexToString;
}

}));

/*!
 * https://github.com/es-shims/es5-shim
 * @license es5-shim Copyright 2009-2015 by contributors, MIT License
 * see https://github.com/es-shims/es5-shim/blob/master/LICENSE
 */

// vim: ts=4 sts=4 sw=4 expandtab

// Add semicolon to prevent IIFE from being passed as argument to concatenated code.
;

// UMD (Universal Module Definition)
// see https://github.com/umdjs/umd/blob/master/templates/returnExports.js
(function (root, factory) {
    'use strict';

    /* global define, exports, module */
    if (typeof define === 'function' && define.amd) {
        // AMD. Register as an anonymous module.
        define(factory);
    } else if (typeof exports === 'object') {
        // Node. Does not work with strict CommonJS, but
        // only CommonJS-like enviroments that support module.exports,
        // like Node.
        module.exports = factory();
    } else {
        // Browser globals (root is window)
        root.returnExports = factory();
    }
}(this, function () {

    var call = Function.call;
    var prototypeOfObject = Object.prototype;
    var owns = call.bind(prototypeOfObject.hasOwnProperty);
    var isEnumerable = call.bind(prototypeOfObject.propertyIsEnumerable);
    var toStr = call.bind(prototypeOfObject.toString);

    // If JS engine supports accessors creating shortcuts.
    var defineGetter;
    var defineSetter;
    var lookupGetter;
    var lookupSetter;
    var supportsAccessors = owns(prototypeOfObject, '__defineGetter__');
    if (supportsAccessors) {
        /* eslint-disable no-underscore-dangle, no-restricted-properties */
        defineGetter = call.bind(prototypeOfObject.__defineGetter__);
        defineSetter = call.bind(prototypeOfObject.__defineSetter__);
        lookupGetter = call.bind(prototypeOfObject.__lookupGetter__);
        lookupSetter = call.bind(prototypeOfObject.__lookupSetter__);
        /* eslint-enable no-underscore-dangle, no-restricted-properties */
    }

    var isPrimitive = function isPrimitive(o) {
        return o == null || (typeof o !== 'object' && typeof o !== 'function');
    };

    // ES5 15.2.3.2
    // http://es5.github.com/#x15.2.3.2
    if (!Object.getPrototypeOf) {
        // https://github.com/es-shims/es5-shim/issues#issue/2
        // http://ejohn.org/blog/objectgetprototypeof/
        // recommended by fschaefer on github
        //
        // sure, and webreflection says ^_^
        // ... this will nerever possibly return null
        // ... Opera Mini breaks here with infinite loops
        Object.getPrototypeOf = function getPrototypeOf(object) {
            // eslint-disable-next-line no-proto
            var proto = object.__proto__;
            if (proto || proto === null) {
                return proto;
            } else if (toStr(object.constructor) === '[object Function]') {
                return object.constructor.prototype;
            } else if (object instanceof Object) {
                return prototypeOfObject;
            } else {
                // Correctly return null for Objects created with `Object.create(null)`
                // (shammed or native) or `{ __proto__: null}`.  Also returns null for
                // cross-realm objects on browsers that lack `__proto__` support (like
                // IE <11), but that's the best we can do.
                return null;
            }
        };
    }

    // ES5 15.2.3.3
    // http://es5.github.com/#x15.2.3.3

    var doesGetOwnPropertyDescriptorWork = function doesGetOwnPropertyDescriptorWork(object) {
        try {
            object.sentinel = 0;
            return Object.getOwnPropertyDescriptor(object, 'sentinel').value === 0;
        } catch (exception) {
            return false;
        }
    };

    // check whether getOwnPropertyDescriptor works if it's given. Otherwise, shim partially.
    if (Object.defineProperty) {
        var getOwnPropertyDescriptorWorksOnObject = doesGetOwnPropertyDescriptorWork({});
        var getOwnPropertyDescriptorWorksOnDom = typeof document === 'undefined' ||
            doesGetOwnPropertyDescriptorWork(document.createElement('div'));
        if (!getOwnPropertyDescriptorWorksOnDom || !getOwnPropertyDescriptorWorksOnObject) {
            var getOwnPropertyDescriptorFallback = Object.getOwnPropertyDescriptor;
        }
    }

    if (!Object.getOwnPropertyDescriptor || getOwnPropertyDescriptorFallback) {
        var ERR_NON_OBJECT = 'Object.getOwnPropertyDescriptor called on a non-object: ';

        /* eslint-disable no-proto */
        Object.getOwnPropertyDescriptor = function getOwnPropertyDescriptor(object, property) {
            if (isPrimitive(object)) {
                throw new TypeError(ERR_NON_OBJECT + object);
            }

            // make a valiant attempt to use the real getOwnPropertyDescriptor
            // for I8's DOM elements.
            if (getOwnPropertyDescriptorFallback) {
                try {
                    return getOwnPropertyDescriptorFallback.call(Object, object, property);
                } catch (exception) {
                    // try the shim if the real one doesn't work
                }
            }

            var descriptor;

            // If object does not owns property return undefined immediately.
            if (!owns(object, property)) {
                return descriptor;
            }

            // If object has a property then it's for sure `configurable`, and
            // probably `enumerable`. Detect enumerability though.
            descriptor = {
                enumerable: isEnumerable(object, property),
                configurable: true
            };

            // If JS engine supports accessor properties then property may be a
            // getter or setter.
            if (supportsAccessors) {
                // Unfortunately `__lookupGetter__` will return a getter even
                // if object has own non getter property along with a same named
                // inherited getter. To avoid misbehavior we temporary remove
                // `__proto__` so that `__lookupGetter__` will return getter only
                // if it's owned by an object.
                var prototype = object.__proto__;
                var notPrototypeOfObject = object !== prototypeOfObject;
                // avoid recursion problem, breaking in Opera Mini when
                // Object.getOwnPropertyDescriptor(Object.prototype, 'toString')
                // or any other Object.prototype accessor
                if (notPrototypeOfObject) {
                    object.__proto__ = prototypeOfObject;
                }

                var getter = lookupGetter(object, property);
                var setter = lookupSetter(object, property);

                if (notPrototypeOfObject) {
                    // Once we have getter and setter we can put values back.
                    object.__proto__ = prototype;
                }

                if (getter || setter) {
                    if (getter) {
                        descriptor.get = getter;
                    }
                    if (setter) {
                        descriptor.set = setter;
                    }
                    // If it was accessor property we're done and return here
                    // in order to avoid adding `value` to the descriptor.
                    return descriptor;
                }
            }

            // If we got this far we know that object has an own property that is
            // not an accessor so we set it as a value and return descriptor.
            descriptor.value = object[property];
            descriptor.writable = true;
            return descriptor;
        };
        /* eslint-enable no-proto */
    }

    // ES5 15.2.3.4
    // http://es5.github.com/#x15.2.3.4
    if (!Object.getOwnPropertyNames) {
        Object.getOwnPropertyNames = function getOwnPropertyNames(object) {
            return Object.keys(object);
        };
    }

    // ES5 15.2.3.5
    // http://es5.github.com/#x15.2.3.5
    if (!Object.create) {

        // Contributed by Brandon Benvie, October, 2012
        var createEmpty;
        var supportsProto = !({ __proto__: null } instanceof Object);
        // the following produces false positives
        // in Opera Mini => not a reliable check
        // Object.prototype.__proto__ === null

        // Check for document.domain and active x support
        // No need to use active x approach when document.domain is not set
        // see https://github.com/es-shims/es5-shim/issues/150
        // variation of https://github.com/kitcambridge/es5-shim/commit/4f738ac066346
        /* global ActiveXObject */
        var shouldUseActiveX = function shouldUseActiveX() {
            // return early if document.domain not set
            if (!document.domain) {
                return false;
            }

            try {
                return !!new ActiveXObject('htmlfile');
            } catch (exception) {
                return false;
            }
        };

        // This supports IE8 when document.domain is used
        // see https://github.com/es-shims/es5-shim/issues/150
        // variation of https://github.com/kitcambridge/es5-shim/commit/4f738ac066346
        var getEmptyViaActiveX = function getEmptyViaActiveX() {
            var empty;
            var xDoc;

            xDoc = new ActiveXObject('htmlfile');

            var script = 'script';
            xDoc.write('<' + script + '></' + script + '>');
            xDoc.close();

            empty = xDoc.parentWindow.Object.prototype;
            xDoc = null;

            return empty;
        };

        // The original implementation using an iframe
        // before the activex approach was added
        // see https://github.com/es-shims/es5-shim/issues/150
        var getEmptyViaIFrame = function getEmptyViaIFrame() {
            var iframe = document.createElement('iframe');
            var parent = document.body || document.documentElement;
            var empty;

            iframe.style.display = 'none';
            parent.appendChild(iframe);
            // eslint-disable-next-line no-script-url
            iframe.src = 'javascript:';

            empty = iframe.contentWindow.Object.prototype;
            parent.removeChild(iframe);
            iframe = null;

            return empty;
        };

        /* global document */
        if (supportsProto || typeof document === 'undefined') {
            createEmpty = function () {
                return { __proto__: null };
            };
        } else {
            // In old IE __proto__ can't be used to manually set `null`, nor does
            // any other method exist to make an object that inherits from nothing,
            // aside from Object.prototype itself. Instead, create a new global
            // object and *steal* its Object.prototype and strip it bare. This is
            // used as the prototype to create nullary objects.
            createEmpty = function () {
                // Determine which approach to use
                // see https://github.com/es-shims/es5-shim/issues/150
                var empty = shouldUseActiveX() ? getEmptyViaActiveX() : getEmptyViaIFrame();

                delete empty.constructor;
                delete empty.hasOwnProperty;
                delete empty.propertyIsEnumerable;
                delete empty.isPrototypeOf;
                delete empty.toLocaleString;
                delete empty.toString;
                delete empty.valueOf;

                var Empty = function Empty() {};
                Empty.prototype = empty;
                // short-circuit future calls
                createEmpty = function () {
                    return new Empty();
                };
                return new Empty();
            };
        }

        Object.create = function create(prototype, properties) {

            var object;
            var Type = function Type() {}; // An empty constructor.

            if (prototype === null) {
                object = createEmpty();
            } else {
                if (prototype !== null && isPrimitive(prototype)) {
                    // In the native implementation `parent` can be `null`
                    // OR *any* `instanceof Object`  (Object|Function|Array|RegExp|etc)
                    // Use `typeof` tho, b/c in old IE, DOM elements are not `instanceof Object`
                    // like they are in modern browsers. Using `Object.create` on DOM elements
                    // is...err...probably inappropriate, but the native version allows for it.
                    throw new TypeError('Object prototype may only be an Object or null'); // same msg as Chrome
                }
                Type.prototype = prototype;
                object = new Type();
                // IE has no built-in implementation of `Object.getPrototypeOf`
                // neither `__proto__`, but this manually setting `__proto__` will
                // guarantee that `Object.getPrototypeOf` will work as expected with
                // objects created using `Object.create`
                // eslint-disable-next-line no-proto
                object.__proto__ = prototype;
            }

            if (properties !== void 0) {
                Object.defineProperties(object, properties);
            }

            return object;
        };
    }

    // ES5 15.2.3.6
    // http://es5.github.com/#x15.2.3.6

    // Patch for WebKit and IE8 standard mode
    // Designed by hax <hax.github.com>
    // related issue: https://github.com/es-shims/es5-shim/issues#issue/5
    // IE8 Reference:
    //     http://msdn.microsoft.com/en-us/library/dd282900.aspx
    //     http://msdn.microsoft.com/en-us/library/dd229916.aspx
    // WebKit Bugs:
    //     https://bugs.webkit.org/show_bug.cgi?id=36423

    var doesDefinePropertyWork = function doesDefinePropertyWork(object) {
        try {
            Object.defineProperty(object, 'sentinel', {});
            return 'sentinel' in object;
        } catch (exception) {
            return false;
        }
    };

    // check whether defineProperty works if it's given. Otherwise,
    // shim partially.
    if (Object.defineProperty) {
        var definePropertyWorksOnObject = doesDefinePropertyWork({});
        var definePropertyWorksOnDom = typeof document === 'undefined' ||
            doesDefinePropertyWork(document.createElement('div'));
        if (!definePropertyWorksOnObject || !definePropertyWorksOnDom) {
            var definePropertyFallback = Object.defineProperty,
                definePropertiesFallback = Object.defineProperties;
        }
    }

    if (!Object.defineProperty || definePropertyFallback) {
        var ERR_NON_OBJECT_DESCRIPTOR = 'Property description must be an object: ';
        var ERR_NON_OBJECT_TARGET = 'Object.defineProperty called on non-object: ';
        var ERR_ACCESSORS_NOT_SUPPORTED = 'getters & setters can not be defined on this javascript engine';

        Object.defineProperty = function defineProperty(object, property, descriptor) {
            if (isPrimitive(object)) {
                throw new TypeError(ERR_NON_OBJECT_TARGET + object);
            }
            if (isPrimitive(descriptor)) {
                throw new TypeError(ERR_NON_OBJECT_DESCRIPTOR + descriptor);
            }
            // make a valiant attempt to use the real defineProperty
            // for I8's DOM elements.
            if (definePropertyFallback) {
                try {
                    return definePropertyFallback.call(Object, object, property, descriptor);
                } catch (exception) {
                    // try the shim if the real one doesn't work
                }
            }

            // If it's a data property.
            if ('value' in descriptor) {
                // fail silently if 'writable', 'enumerable', or 'configurable'
                // are requested but not supported
                /*
                // alternate approach:
                if ( // can't implement these features; allow false but not true
                    ('writable' in descriptor && !descriptor.writable) ||
                    ('enumerable' in descriptor && !descriptor.enumerable) ||
                    ('configurable' in descriptor && !descriptor.configurable)
                ))
                    throw new RangeError(
                        'This implementation of Object.defineProperty does not support configurable, enumerable, or writable.'
                    );
                */

                if (supportsAccessors && (lookupGetter(object, property) || lookupSetter(object, property))) {
                    // As accessors are supported only on engines implementing
                    // `__proto__` we can safely override `__proto__` while defining
                    // a property to make sure that we don't hit an inherited
                    // accessor.
                    /* eslint-disable no-proto */
                    var prototype = object.__proto__;
                    object.__proto__ = prototypeOfObject;
                    // Deleting a property anyway since getter / setter may be
                    // defined on object itself.
                    delete object[property];
                    object[property] = descriptor.value;
                    // Setting original `__proto__` back now.
                    object.__proto__ = prototype;
                    /* eslint-enable no-proto */
                } else {
                    object[property] = descriptor.value;
                }
            } else {
                var hasGetter = 'get' in descriptor;
                var hasSetter = 'set' in descriptor;
                if (!supportsAccessors && (hasGetter || hasSetter)) {
                    throw new TypeError(ERR_ACCESSORS_NOT_SUPPORTED);
                }
                // If we got that far then getters and setters can be defined !!
                if (hasGetter) {
                    defineGetter(object, property, descriptor.get);
                }
                if (hasSetter) {
                    defineSetter(object, property, descriptor.set);
                }
            }
            return object;
        };
    }

    // ES5 15.2.3.7
    // http://es5.github.com/#x15.2.3.7
    if (!Object.defineProperties || definePropertiesFallback) {
        Object.defineProperties = function defineProperties(object, properties) {
            // make a valiant attempt to use the real defineProperties
            if (definePropertiesFallback) {
                try {
                    return definePropertiesFallback.call(Object, object, properties);
                } catch (exception) {
                    // try the shim if the real one doesn't work
                }
            }

            Object.keys(properties).forEach(function (property) {
                if (property !== '__proto__') {
                    Object.defineProperty(object, property, properties[property]);
                }
            });
            return object;
        };
    }

    // ES5 15.2.3.8
    // http://es5.github.com/#x15.2.3.8
    if (!Object.seal) {
        Object.seal = function seal(object) {
            if (Object(object) !== object) {
                throw new TypeError('Object.seal can only be called on Objects.');
            }
            // this is misleading and breaks feature-detection, but
            // allows "securable" code to "gracefully" degrade to working
            // but insecure code.
            return object;
        };
    }

    // ES5 15.2.3.9
    // http://es5.github.com/#x15.2.3.9
    if (!Object.freeze) {
        Object.freeze = function freeze(object) {
            if (Object(object) !== object) {
                throw new TypeError('Object.freeze can only be called on Objects.');
            }
            // this is misleading and breaks feature-detection, but
            // allows "securable" code to "gracefully" degrade to working
            // but insecure code.
            return object;
        };
    }

    // detect a Rhino bug and patch it
    try {
        Object.freeze(function () {});
    } catch (exception) {
        Object.freeze = (function (freezeObject) {
            return function freeze(object) {
                if (typeof object === 'function') {
                    return object;
                } else {
                    return freezeObject(object);
                }
            };
        }(Object.freeze));
    }

    // ES5 15.2.3.10
    // http://es5.github.com/#x15.2.3.10
    if (!Object.preventExtensions) {
        Object.preventExtensions = function preventExtensions(object) {
            if (Object(object) !== object) {
                throw new TypeError('Object.preventExtensions can only be called on Objects.');
            }
            // this is misleading and breaks feature-detection, but
            // allows "securable" code to "gracefully" degrade to working
            // but insecure code.
            return object;
        };
    }

    // ES5 15.2.3.11
    // http://es5.github.com/#x15.2.3.11
    if (!Object.isSealed) {
        Object.isSealed = function isSealed(object) {
            if (Object(object) !== object) {
                throw new TypeError('Object.isSealed can only be called on Objects.');
            }
            return false;
        };
    }

    // ES5 15.2.3.12
    // http://es5.github.com/#x15.2.3.12
    if (!Object.isFrozen) {
        Object.isFrozen = function isFrozen(object) {
            if (Object(object) !== object) {
                throw new TypeError('Object.isFrozen can only be called on Objects.');
            }
            return false;
        };
    }

    // ES5 15.2.3.13
    // http://es5.github.com/#x15.2.3.13
    if (!Object.isExtensible) {
        Object.isExtensible = function isExtensible(object) {
            // 1. If Type(O) is not Object throw a TypeError exception.
            if (Object(object) !== object) {
                throw new TypeError('Object.isExtensible can only be called on Objects.');
            }
            // 2. Return the Boolean value of the [[Extensible]] internal property of O.
            var name = '';
            while (owns(object, name)) {
                name += '?';
            }
            object[name] = true;
            var returnValue = owns(object, name);
            delete object[name];
            return returnValue;
        };
    }

}));
/*!
 * jQuery JavaScript Library v3.6.0
 * https://jquery.com/
 *
 * Includes Sizzle.js
 * https://sizzlejs.com/
 *
 * Copyright OpenJS Foundation and other contributors
 * Released under the MIT license
 * https://jquery.org/license
 *
 * Date: 2021-03-02T17:08Z
 */
( function( global, factory ) {

	"use strict";

	if ( typeof module === "object" && typeof module.exports === "object" ) {

		// For CommonJS and CommonJS-like environments where a proper `window`
		// is present, execute the factory and get jQuery.
		// For environments that do not have a `window` with a `document`
		// (such as Node.js), expose a factory as module.exports.
		// This accentuates the need for the creation of a real `window`.
		// e.g. var jQuery = require("jquery")(window);
		// See ticket #14549 for more info.
		module.exports = global.document ?
			factory( global, true ) :
			function( w ) {
				if ( !w.document ) {
					throw new Error( "jQuery requires a window with a document" );
				}
				return factory( w );
			};
	} else {
		factory( global );
	}

// Pass this if window is not defined yet
} )( typeof window !== "undefined" ? window : this, function( window, noGlobal ) {

// Edge <= 12 - 13+, Firefox <=18 - 45+, IE 10 - 11, Safari 5.1 - 9+, iOS 6 - 9.1
// throw exceptions when non-strict code (e.g., ASP.NET 4.5) accesses strict mode
// arguments.callee.caller (trac-13335). But as of jQuery 3.0 (2016), strict mode should be common
// enough that all such attempts are guarded in a try block.
"use strict";

var arr = [];

var getProto = Object.getPrototypeOf;

var slice = arr.slice;

var flat = arr.flat ? function( array ) {
	return arr.flat.call( array );
} : function( array ) {
	return arr.concat.apply( [], array );
};


var push = arr.push;

var indexOf = arr.indexOf;

var class2type = {};

var toString = class2type.toString;

var hasOwn = class2type.hasOwnProperty;

var fnToString = hasOwn.toString;

var ObjectFunctionString = fnToString.call( Object );

var support = {};

var isFunction = function isFunction( obj ) {

		// Support: Chrome <=57, Firefox <=52
		// In some browsers, typeof returns "function" for HTML <object> elements
		// (i.e., `typeof document.createElement( "object" ) === "function"`).
		// We don't want to classify *any* DOM node as a function.
		// Support: QtWeb <=3.8.5, WebKit <=534.34, wkhtmltopdf tool <=0.12.5
		// Plus for old WebKit, typeof returns "function" for HTML collections
		// (e.g., `typeof document.getElementsByTagName("div") === "function"`). (gh-4756)
		return typeof obj === "function" && typeof obj.nodeType !== "number" &&
			typeof obj.item !== "function";
	};


var isWindow = function isWindow( obj ) {
		return obj != null && obj === obj.window;
	};


var document = window.document;



	var preservedScriptAttributes = {
		type: true,
		src: true,
		nonce: true,
		noModule: true
	};

	function DOMEval( code, node, doc ) {
		doc = doc || document;

		var i, val,
			script = doc.createElement( "script" );

		script.text = code;
		if ( node ) {
			for ( i in preservedScriptAttributes ) {

				// Support: Firefox 64+, Edge 18+
				// Some browsers don't support the "nonce" property on scripts.
				// On the other hand, just using `getAttribute` is not enough as
				// the `nonce` attribute is reset to an empty string whenever it
				// becomes browsing-context connected.
				// See https://github.com/whatwg/html/issues/2369
				// See https://html.spec.whatwg.org/#nonce-attributes
				// The `node.getAttribute` check was added for the sake of
				// `jQuery.globalEval` so that it can fake a nonce-containing node
				// via an object.
				val = node[ i ] || node.getAttribute && node.getAttribute( i );
				if ( val ) {
					script.setAttribute( i, val );
				}
			}
		}
		doc.head.appendChild( script ).parentNode.removeChild( script );
	}


function toType( obj ) {
	if ( obj == null ) {
		return obj + "";
	}

	// Support: Android <=2.3 only (functionish RegExp)
	return typeof obj === "object" || typeof obj === "function" ?
		class2type[ toString.call( obj ) ] || "object" :
		typeof obj;
}
/* global Symbol */
// Defining this global in .eslintrc.json would create a danger of using the global
// unguarded in another place, it seems safer to define global only for this module



var
	version = "3.6.0",

	// Define a local copy of jQuery
	jQuery = function( selector, context ) {

		// The jQuery object is actually just the init constructor 'enhanced'
		// Need init if jQuery is called (just allow error to be thrown if not included)
		return new jQuery.fn.init( selector, context );
	};

jQuery.fn = jQuery.prototype = {

	// The current version of jQuery being used
	jquery: version,

	constructor: jQuery,

	// The default length of a jQuery object is 0
	length: 0,

	toArray: function() {
		return slice.call( this );
	},

	// Get the Nth element in the matched element set OR
	// Get the whole matched element set as a clean array
	get: function( num ) {

		// Return all the elements in a clean array
		if ( num == null ) {
			return slice.call( this );
		}

		// Return just the one element from the set
		return num < 0 ? this[ num + this.length ] : this[ num ];
	},

	// Take an array of elements and push it onto the stack
	// (returning the new matched element set)
	pushStack: function( elems ) {

		// Build a new jQuery matched element set
		var ret = jQuery.merge( this.constructor(), elems );

		// Add the old object onto the stack (as a reference)
		ret.prevObject = this;

		// Return the newly-formed element set
		return ret;
	},

	// Execute a callback for every element in the matched set.
	each: function( callback ) {
		return jQuery.each( this, callback );
	},

	map: function( callback ) {
		return this.pushStack( jQuery.map( this, function( elem, i ) {
			return callback.call( elem, i, elem );
		} ) );
	},

	slice: function() {
		return this.pushStack( slice.apply( this, arguments ) );
	},

	first: function() {
		return this.eq( 0 );
	},

	last: function() {
		return this.eq( -1 );
	},

	even: function() {
		return this.pushStack( jQuery.grep( this, function( _elem, i ) {
			return ( i + 1 ) % 2;
		} ) );
	},

	odd: function() {
		return this.pushStack( jQuery.grep( this, function( _elem, i ) {
			return i % 2;
		} ) );
	},

	eq: function( i ) {
		var len = this.length,
			j = +i + ( i < 0 ? len : 0 );
		return this.pushStack( j >= 0 && j < len ? [ this[ j ] ] : [] );
	},

	end: function() {
		return this.prevObject || this.constructor();
	},

	// For internal use only.
	// Behaves like an Array's method, not like a jQuery method.
	push: push,
	sort: arr.sort,
	splice: arr.splice
};

jQuery.extend = jQuery.fn.extend = function() {
	var options, name, src, copy, copyIsArray, clone,
		target = arguments[ 0 ] || {},
		i = 1,
		length = arguments.length,
		deep = false;

	// Handle a deep copy situation
	if ( typeof target === "boolean" ) {
		deep = target;

		// Skip the boolean and the target
		target = arguments[ i ] || {};
		i++;
	}

	// Handle case when target is a string or something (possible in deep copy)
	if ( typeof target !== "object" && !isFunction( target ) ) {
		target = {};
	}

	// Extend jQuery itself if only one argument is passed
	if ( i === length ) {
		target = this;
		i--;
	}

	for ( ; i < length; i++ ) {

		// Only deal with non-null/undefined values
		if ( ( options = arguments[ i ] ) != null ) {

			// Extend the base object
			for ( name in options ) {
				copy = options[ name ];

				// Prevent Object.prototype pollution
				// Prevent never-ending loop
				if ( name === "__proto__" || target === copy ) {
					continue;
				}

				// Recurse if we're merging plain objects or arrays
				if ( deep && copy && ( jQuery.isPlainObject( copy ) ||
					( copyIsArray = Array.isArray( copy ) ) ) ) {
					src = target[ name ];

					// Ensure proper type for the source value
					if ( copyIsArray && !Array.isArray( src ) ) {
						clone = [];
					} else if ( !copyIsArray && !jQuery.isPlainObject( src ) ) {
						clone = {};
					} else {
						clone = src;
					}
					copyIsArray = false;

					// Never move original objects, clone them
					target[ name ] = jQuery.extend( deep, clone, copy );

				// Don't bring in undefined values
				} else if ( copy !== undefined ) {
					target[ name ] = copy;
				}
			}
		}
	}

	// Return the modified object
	return target;
};

jQuery.extend( {

	// Unique for each copy of jQuery on the page
	expando: "jQuery" + ( version + Math.random() ).replace( /\D/g, "" ),

	// Assume jQuery is ready without the ready module
	isReady: true,

	error: function( msg ) {
		throw new Error( msg );
	},

	noop: function() {},

	isPlainObject: function( obj ) {
		var proto, Ctor;

		// Detect obvious negatives
		// Use toString instead of jQuery.type to catch host objects
		if ( !obj || toString.call( obj ) !== "[object Object]" ) {
			return false;
		}

		proto = getProto( obj );

		// Objects with no prototype (e.g., `Object.create( null )`) are plain
		if ( !proto ) {
			return true;
		}

		// Objects with prototype are plain iff they were constructed by a global Object function
		Ctor = hasOwn.call( proto, "constructor" ) && proto.constructor;
		return typeof Ctor === "function" && fnToString.call( Ctor ) === ObjectFunctionString;
	},

	isEmptyObject: function( obj ) {
		var name;

		for ( name in obj ) {
			return false;
		}
		return true;
	},

	// Evaluates a script in a provided context; falls back to the global one
	// if not specified.
	globalEval: function( code, options, doc ) {
		DOMEval( code, { nonce: options && options.nonce }, doc );
	},

	each: function( obj, callback ) {
		var length, i = 0;

		if ( isArrayLike( obj ) ) {
			length = obj.length;
			for ( ; i < length; i++ ) {
				if ( callback.call( obj[ i ], i, obj[ i ] ) === false ) {
					break;
				}
			}
		} else {
			for ( i in obj ) {
				if ( callback.call( obj[ i ], i, obj[ i ] ) === false ) {
					break;
				}
			}
		}

		return obj;
	},

	// results is for internal usage only
	makeArray: function( arr, results ) {
		var ret = results || [];

		if ( arr != null ) {
			if ( isArrayLike( Object( arr ) ) ) {
				jQuery.merge( ret,
					typeof arr === "string" ?
						[ arr ] : arr
				);
			} else {
				push.call( ret, arr );
			}
		}

		return ret;
	},

	inArray: function( elem, arr, i ) {
		return arr == null ? -1 : indexOf.call( arr, elem, i );
	},

	// Support: Android <=4.0 only, PhantomJS 1 only
	// push.apply(_, arraylike) throws on ancient WebKit
	merge: function( first, second ) {
		var len = +second.length,
			j = 0,
			i = first.length;

		for ( ; j < len; j++ ) {
			first[ i++ ] = second[ j ];
		}

		first.length = i;

		return first;
	},

	grep: function( elems, callback, invert ) {
		var callbackInverse,
			matches = [],
			i = 0,
			length = elems.length,
			callbackExpect = !invert;

		// Go through the array, only saving the items
		// that pass the validator function
		for ( ; i < length; i++ ) {
			callbackInverse = !callback( elems[ i ], i );
			if ( callbackInverse !== callbackExpect ) {
				matches.push( elems[ i ] );
			}
		}

		return matches;
	},

	// arg is for internal usage only
	map: function( elems, callback, arg ) {
		var length, value,
			i = 0,
			ret = [];

		// Go through the array, translating each of the items to their new values
		if ( isArrayLike( elems ) ) {
			length = elems.length;
			for ( ; i < length; i++ ) {
				value = callback( elems[ i ], i, arg );

				if ( value != null ) {
					ret.push( value );
				}
			}

		// Go through every key on the object,
		} else {
			for ( i in elems ) {
				value = callback( elems[ i ], i, arg );

				if ( value != null ) {
					ret.push( value );
				}
			}
		}

		// Flatten any nested arrays
		return flat( ret );
	},

	// A global GUID counter for objects
	guid: 1,

	// jQuery.support is not used in Core but other projects attach their
	// properties to it so it needs to exist.
	support: support
} );

if ( typeof Symbol === "function" ) {
	jQuery.fn[ Symbol.iterator ] = arr[ Symbol.iterator ];
}

// Populate the class2type map
jQuery.each( "Boolean Number String Function Array Date RegExp Object Error Symbol".split( " " ),
	function( _i, name ) {
		class2type[ "[object " + name + "]" ] = name.toLowerCase();
	} );

function isArrayLike( obj ) {

	// Support: real iOS 8.2 only (not reproducible in simulator)
	// `in` check used to prevent JIT error (gh-2145)
	// hasOwn isn't used here due to false negatives
	// regarding Nodelist length in IE
	var length = !!obj && "length" in obj && obj.length,
		type = toType( obj );

	if ( isFunction( obj ) || isWindow( obj ) ) {
		return false;
	}

	return type === "array" || length === 0 ||
		typeof length === "number" && length > 0 && ( length - 1 ) in obj;
}
var Sizzle =
/*!
 * Sizzle CSS Selector Engine v2.3.6
 * https://sizzlejs.com/
 *
 * Copyright JS Foundation and other contributors
 * Released under the MIT license
 * https://js.foundation/
 *
 * Date: 2021-02-16
 */
( function( window ) {
var i,
	support,
	Expr,
	getText,
	isXML,
	tokenize,
	compile,
	select,
	outermostContext,
	sortInput,
	hasDuplicate,

	// Local document vars
	setDocument,
	document,
	docElem,
	documentIsHTML,
	rbuggyQSA,
	rbuggyMatches,
	matches,
	contains,

	// Instance-specific data
	expando = "sizzle" + 1 * new Date(),
	preferredDoc = window.document,
	dirruns = 0,
	done = 0,
	classCache = createCache(),
	tokenCache = createCache(),
	compilerCache = createCache(),
	nonnativeSelectorCache = createCache(),
	sortOrder = function( a, b ) {
		if ( a === b ) {
			hasDuplicate = true;
		}
		return 0;
	},

	// Instance methods
	hasOwn = ( {} ).hasOwnProperty,
	arr = [],
	pop = arr.pop,
	pushNative = arr.push,
	push = arr.push,
	slice = arr.slice,

	// Use a stripped-down indexOf as it's faster than native
	// https://jsperf.com/thor-indexof-vs-for/5
	indexOf = function( list, elem ) {
		var i = 0,
			len = list.length;
		for ( ; i < len; i++ ) {
			if ( list[ i ] === elem ) {
				return i;
			}
		}
		return -1;
	},

	booleans = "checked|selected|async|autofocus|autoplay|controls|defer|disabled|hidden|" +
		"ismap|loop|multiple|open|readonly|required|scoped",

	// Regular expressions

	// http://www.w3.org/TR/css3-selectors/#whitespace
	whitespace = "[\\x20\\t\\r\\n\\f]",

	// https://www.w3.org/TR/css-syntax-3/#ident-token-diagram
	identifier = "(?:\\\\[\\da-fA-F]{1,6}" + whitespace +
		"?|\\\\[^\\r\\n\\f]|[\\w-]|[^\0-\\x7f])+",

	// Attribute selectors: http://www.w3.org/TR/selectors/#attribute-selectors
	attributes = "\\[" + whitespace + "*(" + identifier + ")(?:" + whitespace +

		// Operator (capture 2)
		"*([*^$|!~]?=)" + whitespace +

		// "Attribute values must be CSS identifiers [capture 5]
		// or strings [capture 3 or capture 4]"
		"*(?:'((?:\\\\.|[^\\\\'])*)'|\"((?:\\\\.|[^\\\\\"])*)\"|(" + identifier + "))|)" +
		whitespace + "*\\]",

	pseudos = ":(" + identifier + ")(?:\\((" +

		// To reduce the number of selectors needing tokenize in the preFilter, prefer arguments:
		// 1. quoted (capture 3; capture 4 or capture 5)
		"('((?:\\\\.|[^\\\\'])*)'|\"((?:\\\\.|[^\\\\\"])*)\")|" +

		// 2. simple (capture 6)
		"((?:\\\\.|[^\\\\()[\\]]|" + attributes + ")*)|" +

		// 3. anything else (capture 2)
		".*" +
		")\\)|)",

	// Leading and non-escaped trailing whitespace, capturing some non-whitespace characters preceding the latter
	rwhitespace = new RegExp( whitespace + "+", "g" ),
	rtrim = new RegExp( "^" + whitespace + "+|((?:^|[^\\\\])(?:\\\\.)*)" +
		whitespace + "+$", "g" ),

	rcomma = new RegExp( "^" + whitespace + "*," + whitespace + "*" ),
	rcombinators = new RegExp( "^" + whitespace + "*([>+~]|" + whitespace + ")" + whitespace +
		"*" ),
	rdescend = new RegExp( whitespace + "|>" ),

	rpseudo = new RegExp( pseudos ),
	ridentifier = new RegExp( "^" + identifier + "$" ),

	matchExpr = {
		"ID": new RegExp( "^#(" + identifier + ")" ),
		"CLASS": new RegExp( "^\\.(" + identifier + ")" ),
		"TAG": new RegExp( "^(" + identifier + "|[*])" ),
		"ATTR": new RegExp( "^" + attributes ),
		"PSEUDO": new RegExp( "^" + pseudos ),
		"CHILD": new RegExp( "^:(only|first|last|nth|nth-last)-(child|of-type)(?:\\(" +
			whitespace + "*(even|odd|(([+-]|)(\\d*)n|)" + whitespace + "*(?:([+-]|)" +
			whitespace + "*(\\d+)|))" + whitespace + "*\\)|)", "i" ),
		"bool": new RegExp( "^(?:" + booleans + ")$", "i" ),

		// For use in libraries implementing .is()
		// We use this for POS matching in `select`
		"needsContext": new RegExp( "^" + whitespace +
			"*[>+~]|:(even|odd|eq|gt|lt|nth|first|last)(?:\\(" + whitespace +
			"*((?:-\\d)?\\d*)" + whitespace + "*\\)|)(?=[^-]|$)", "i" )
	},

	rhtml = /HTML$/i,
	rinputs = /^(?:input|select|textarea|button)$/i,
	rheader = /^h\d$/i,

	rnative = /^[^{]+\{\s*\[native \w/,

	// Easily-parseable/retrievable ID or TAG or CLASS selectors
	rquickExpr = /^(?:#([\w-]+)|(\w+)|\.([\w-]+))$/,

	rsibling = /[+~]/,

	// CSS escapes
	// http://www.w3.org/TR/CSS21/syndata.html#escaped-characters
	runescape = new RegExp( "\\\\[\\da-fA-F]{1,6}" + whitespace + "?|\\\\([^\\r\\n\\f])", "g" ),
	funescape = function( escape, nonHex ) {
		var high = "0x" + escape.slice( 1 ) - 0x10000;

		return nonHex ?

			// Strip the backslash prefix from a non-hex escape sequence
			nonHex :

			// Replace a hexadecimal escape sequence with the encoded Unicode code point
			// Support: IE <=11+
			// For values outside the Basic Multilingual Plane (BMP), manually construct a
			// surrogate pair
			high < 0 ?
				String.fromCharCode( high + 0x10000 ) :
				String.fromCharCode( high >> 10 | 0xD800, high & 0x3FF | 0xDC00 );
	},

	// CSS string/identifier serialization
	// https://drafts.csswg.org/cssom/#common-serializing-idioms
	rcssescape = /([\0-\x1f\x7f]|^-?\d)|^-$|[^\0-\x1f\x7f-\uFFFF\w-]/g,
	fcssescape = function( ch, asCodePoint ) {
		if ( asCodePoint ) {

			// U+0000 NULL becomes U+FFFD REPLACEMENT CHARACTER
			if ( ch === "\0" ) {
				return "\uFFFD";
			}

			// Control characters and (dependent upon position) numbers get escaped as code points
			return ch.slice( 0, -1 ) + "\\" +
				ch.charCodeAt( ch.length - 1 ).toString( 16 ) + " ";
		}

		// Other potentially-special ASCII characters get backslash-escaped
		return "\\" + ch;
	},

	// Used for iframes
	// See setDocument()
	// Removing the function wrapper causes a "Permission Denied"
	// error in IE
	unloadHandler = function() {
		setDocument();
	},

	inDisabledFieldset = addCombinator(
		function( elem ) {
			return elem.disabled === true && elem.nodeName.toLowerCase() === "fieldset";
		},
		{ dir: "parentNode", next: "legend" }
	);

// Optimize for push.apply( _, NodeList )
try {
	push.apply(
		( arr = slice.call( preferredDoc.childNodes ) ),
		preferredDoc.childNodes
	);

	// Support: Android<4.0
	// Detect silently failing push.apply
	// eslint-disable-next-line no-unused-expressions
	arr[ preferredDoc.childNodes.length ].nodeType;
} catch ( e ) {
	push = { apply: arr.length ?

		// Leverage slice if possible
		function( target, els ) {
			pushNative.apply( target, slice.call( els ) );
		} :

		// Support: IE<9
		// Otherwise append directly
		function( target, els ) {
			var j = target.length,
				i = 0;

			// Can't trust NodeList.length
			while ( ( target[ j++ ] = els[ i++ ] ) ) {}
			target.length = j - 1;
		}
	};
}

function Sizzle( selector, context, results, seed ) {
	var m, i, elem, nid, match, groups, newSelector,
		newContext = context && context.ownerDocument,

		// nodeType defaults to 9, since context defaults to document
		nodeType = context ? context.nodeType : 9;

	results = results || [];

	// Return early from calls with invalid selector or context
	if ( typeof selector !== "string" || !selector ||
		nodeType !== 1 && nodeType !== 9 && nodeType !== 11 ) {

		return results;
	}

	// Try to shortcut find operations (as opposed to filters) in HTML documents
	if ( !seed ) {
		setDocument( context );
		context = context || document;

		if ( documentIsHTML ) {

			// If the selector is sufficiently simple, try using a "get*By*" DOM method
			// (excepting DocumentFragment context, where the methods don't exist)
			if ( nodeType !== 11 && ( match = rquickExpr.exec( selector ) ) ) {

				// ID selector
				if ( ( m = match[ 1 ] ) ) {

					// Document context
					if ( nodeType === 9 ) {
						if ( ( elem = context.getElementById( m ) ) ) {

							// Support: IE, Opera, Webkit
							// TODO: identify versions
							// getElementById can match elements by name instead of ID
							if ( elem.id === m ) {
								results.push( elem );
								return results;
							}
						} else {
							return results;
						}

					// Element context
					} else {

						// Support: IE, Opera, Webkit
						// TODO: identify versions
						// getElementById can match elements by name instead of ID
						if ( newContext && ( elem = newContext.getElementById( m ) ) &&
							contains( context, elem ) &&
							elem.id === m ) {

							results.push( elem );
							return results;
						}
					}

				// Type selector
				} else if ( match[ 2 ] ) {
					push.apply( results, context.getElementsByTagName( selector ) );
					return results;

				// Class selector
				} else if ( ( m = match[ 3 ] ) && support.getElementsByClassName &&
					context.getElementsByClassName ) {

					push.apply( results, context.getElementsByClassName( m ) );
					return results;
				}
			}

			// Take advantage of querySelectorAll
			if ( support.qsa &&
				!nonnativeSelectorCache[ selector + " " ] &&
				( !rbuggyQSA || !rbuggyQSA.test( selector ) ) &&

				// Support: IE 8 only
				// Exclude object elements
				( nodeType !== 1 || context.nodeName.toLowerCase() !== "object" ) ) {

				newSelector = selector;
				newContext = context;

				// qSA considers elements outside a scoping root when evaluating child or
				// descendant combinators, which is not what we want.
				// In such cases, we work around the behavior by prefixing every selector in the
				// list with an ID selector referencing the scope context.
				// The technique has to be used as well when a leading combinator is used
				// as such selectors are not recognized by querySelectorAll.
				// Thanks to Andrew Dupont for this technique.
				if ( nodeType === 1 &&
					( rdescend.test( selector ) || rcombinators.test( selector ) ) ) {

					// Expand context for sibling selectors
					newContext = rsibling.test( selector ) && testContext( context.parentNode ) ||
						context;

					// We can use :scope instead of the ID hack if the browser
					// supports it & if we're not changing the context.
					if ( newContext !== context || !support.scope ) {

						// Capture the context ID, setting it first if necessary
						if ( ( nid = context.getAttribute( "id" ) ) ) {
							nid = nid.replace( rcssescape, fcssescape );
						} else {
							context.setAttribute( "id", ( nid = expando ) );
						}
					}

					// Prefix every selector in the list
					groups = tokenize( selector );
					i = groups.length;
					while ( i-- ) {
						groups[ i ] = ( nid ? "#" + nid : ":scope" ) + " " +
							toSelector( groups[ i ] );
					}
					newSelector = groups.join( "," );
				}

				try {
					push.apply( results,
						newContext.querySelectorAll( newSelector )
					);
					return results;
				} catch ( qsaError ) {
					nonnativeSelectorCache( selector, true );
				} finally {
					if ( nid === expando ) {
						context.removeAttribute( "id" );
					}
				}
			}
		}
	}

	// All others
	return select( selector.replace( rtrim, "$1" ), context, results, seed );
}

/**
 * Create key-value caches of limited size
 * @returns {function(string, object)} Returns the Object data after storing it on itself with
 *	property name the (space-suffixed) string and (if the cache is larger than Expr.cacheLength)
 *	deleting the oldest entry
 */
function createCache() {
	var keys = [];

	function cache( key, value ) {

		// Use (key + " ") to avoid collision with native prototype properties (see Issue #157)
		if ( keys.push( key + " " ) > Expr.cacheLength ) {

			// Only keep the most recent entries
			delete cache[ keys.shift() ];
		}
		return ( cache[ key + " " ] = value );
	}
	return cache;
}

/**
 * Mark a function for special use by Sizzle
 * @param {Function} fn The function to mark
 */
function markFunction( fn ) {
	fn[ expando ] = true;
	return fn;
}

/**
 * Support testing using an element
 * @param {Function} fn Passed the created element and returns a boolean result
 */
function assert( fn ) {
	var el = document.createElement( "fieldset" );

	try {
		return !!fn( el );
	} catch ( e ) {
		return false;
	} finally {

		// Remove from its parent by default
		if ( el.parentNode ) {
			el.parentNode.removeChild( el );
		}

		// release memory in IE
		el = null;
	}
}

/**
 * Adds the same handler for all of the specified attrs
 * @param {String} attrs Pipe-separated list of attributes
 * @param {Function} handler The method that will be applied
 */
function addHandle( attrs, handler ) {
	var arr = attrs.split( "|" ),
		i = arr.length;

	while ( i-- ) {
		Expr.attrHandle[ arr[ i ] ] = handler;
	}
}

/**
 * Checks document order of two siblings
 * @param {Element} a
 * @param {Element} b
 * @returns {Number} Returns less than 0 if a precedes b, greater than 0 if a follows b
 */
function siblingCheck( a, b ) {
	var cur = b && a,
		diff = cur && a.nodeType === 1 && b.nodeType === 1 &&
			a.sourceIndex - b.sourceIndex;

	// Use IE sourceIndex if available on both nodes
	if ( diff ) {
		return diff;
	}

	// Check if b follows a
	if ( cur ) {
		while ( ( cur = cur.nextSibling ) ) {
			if ( cur === b ) {
				return -1;
			}
		}
	}

	return a ? 1 : -1;
}

/**
 * Returns a function to use in pseudos for input types
 * @param {String} type
 */
function createInputPseudo( type ) {
	return function( elem ) {
		var name = elem.nodeName.toLowerCase();
		return name === "input" && elem.type === type;
	};
}

/**
 * Returns a function to use in pseudos for buttons
 * @param {String} type
 */
function createButtonPseudo( type ) {
	return function( elem ) {
		var name = elem.nodeName.toLowerCase();
		return ( name === "input" || name === "button" ) && elem.type === type;
	};
}

/**
 * Returns a function to use in pseudos for :enabled/:disabled
 * @param {Boolean} disabled true for :disabled; false for :enabled
 */
function createDisabledPseudo( disabled ) {

	// Known :disabled false positives: fieldset[disabled] > legend:nth-of-type(n+2) :can-disable
	return function( elem ) {

		// Only certain elements can match :enabled or :disabled
		// https://html.spec.whatwg.org/multipage/scripting.html#selector-enabled
		// https://html.spec.whatwg.org/multipage/scripting.html#selector-disabled
		if ( "form" in elem ) {

			// Check for inherited disabledness on relevant non-disabled elements:
			// * listed form-associated elements in a disabled fieldset
			//   https://html.spec.whatwg.org/multipage/forms.html#category-listed
			//   https://html.spec.whatwg.org/multipage/forms.html#concept-fe-disabled
			// * option elements in a disabled optgroup
			//   https://html.spec.whatwg.org/multipage/forms.html#concept-option-disabled
			// All such elements have a "form" property.
			if ( elem.parentNode && elem.disabled === false ) {

				// Option elements defer to a parent optgroup if present
				if ( "label" in elem ) {
					if ( "label" in elem.parentNode ) {
						return elem.parentNode.disabled === disabled;
					} else {
						return elem.disabled === disabled;
					}
				}

				// Support: IE 6 - 11
				// Use the isDisabled shortcut property to check for disabled fieldset ancestors
				return elem.isDisabled === disabled ||

					// Where there is no isDisabled, check manually
					/* jshint -W018 */
					elem.isDisabled !== !disabled &&
					inDisabledFieldset( elem ) === disabled;
			}

			return elem.disabled === disabled;

		// Try to winnow out elements that can't be disabled before trusting the disabled property.
		// Some victims get caught in our net (label, legend, menu, track), but it shouldn't
		// even exist on them, let alone have a boolean value.
		} else if ( "label" in elem ) {
			return elem.disabled === disabled;
		}

		// Remaining elements are neither :enabled nor :disabled
		return false;
	};
}

/**
 * Returns a function to use in pseudos for positionals
 * @param {Function} fn
 */
function createPositionalPseudo( fn ) {
	return markFunction( function( argument ) {
		argument = +argument;
		return markFunction( function( seed, matches ) {
			var j,
				matchIndexes = fn( [], seed.length, argument ),
				i = matchIndexes.length;

			// Match elements found at the specified indexes
			while ( i-- ) {
				if ( seed[ ( j = matchIndexes[ i ] ) ] ) {
					seed[ j ] = !( matches[ j ] = seed[ j ] );
				}
			}
		} );
	} );
}

/**
 * Checks a node for validity as a Sizzle context
 * @param {Element|Object=} context
 * @returns {Element|Object|Boolean} The input node if acceptable, otherwise a falsy value
 */
function testContext( context ) {
	return context && typeof context.getElementsByTagName !== "undefined" && context;
}

// Expose support vars for convenience
support = Sizzle.support = {};

/**
 * Detects XML nodes
 * @param {Element|Object} elem An element or a document
 * @returns {Boolean} True iff elem is a non-HTML XML node
 */
isXML = Sizzle.isXML = function( elem ) {
	var namespace = elem && elem.namespaceURI,
		docElem = elem && ( elem.ownerDocument || elem ).documentElement;

	// Support: IE <=8
	// Assume HTML when documentElement doesn't yet exist, such as inside loading iframes
	// https://bugs.jquery.com/ticket/4833
	return !rhtml.test( namespace || docElem && docElem.nodeName || "HTML" );
};

/**
 * Sets document-related variables once based on the current document
 * @param {Element|Object} [doc] An element or document object to use to set the document
 * @returns {Object} Returns the current document
 */
setDocument = Sizzle.setDocument = function( node ) {
	var hasCompare, subWindow,
		doc = node ? node.ownerDocument || node : preferredDoc;

	// Return early if doc is invalid or already selected
	// Support: IE 11+, Edge 17 - 18+
	// IE/Edge sometimes throw a "Permission denied" error when strict-comparing
	// two documents; shallow comparisons work.
	// eslint-disable-next-line eqeqeq
	if ( doc == document || doc.nodeType !== 9 || !doc.documentElement ) {
		return document;
	}

	// Update global variables
	document = doc;
	docElem = document.documentElement;
	documentIsHTML = !isXML( document );

	// Support: IE 9 - 11+, Edge 12 - 18+
	// Accessing iframe documents after unload throws "permission denied" errors (jQuery #13936)
	// Support: IE 11+, Edge 17 - 18+
	// IE/Edge sometimes throw a "Permission denied" error when strict-comparing
	// two documents; shallow comparisons work.
	// eslint-disable-next-line eqeqeq
	if ( preferredDoc != document &&
		( subWindow = document.defaultView ) && subWindow.top !== subWindow ) {

		// Support: IE 11, Edge
		if ( subWindow.addEventListener ) {
			subWindow.addEventListener( "unload", unloadHandler, false );

		// Support: IE 9 - 10 only
		} else if ( subWindow.attachEvent ) {
			subWindow.attachEvent( "onunload", unloadHandler );
		}
	}

	// Support: IE 8 - 11+, Edge 12 - 18+, Chrome <=16 - 25 only, Firefox <=3.6 - 31 only,
	// Safari 4 - 5 only, Opera <=11.6 - 12.x only
	// IE/Edge & older browsers don't support the :scope pseudo-class.
	// Support: Safari 6.0 only
	// Safari 6.0 supports :scope but it's an alias of :root there.
	support.scope = assert( function( el ) {
		docElem.appendChild( el ).appendChild( document.createElement( "div" ) );
		return typeof el.querySelectorAll !== "undefined" &&
			!el.querySelectorAll( ":scope fieldset div" ).length;
	} );

	/* Attributes
	---------------------------------------------------------------------- */

	// Support: IE<8
	// Verify that getAttribute really returns attributes and not properties
	// (excepting IE8 booleans)
	support.attributes = assert( function( el ) {
		el.className = "i";
		return !el.getAttribute( "className" );
	} );

	/* getElement(s)By*
	---------------------------------------------------------------------- */

	// Check if getElementsByTagName("*") returns only elements
	support.getElementsByTagName = assert( function( el ) {
		el.appendChild( document.createComment( "" ) );
		return !el.getElementsByTagName( "*" ).length;
	} );

	// Support: IE<9
	support.getElementsByClassName = rnative.test( document.getElementsByClassName );

	// Support: IE<10
	// Check if getElementById returns elements by name
	// The broken getElementById methods don't pick up programmatically-set names,
	// so use a roundabout getElementsByName test
	support.getById = assert( function( el ) {
		docElem.appendChild( el ).id = expando;
		return !document.getElementsByName || !document.getElementsByName( expando ).length;
	} );

	// ID filter and find
	if ( support.getById ) {
		Expr.filter[ "ID" ] = function( id ) {
			var attrId = id.replace( runescape, funescape );
			return function( elem ) {
				return elem.getAttribute( "id" ) === attrId;
			};
		};
		Expr.find[ "ID" ] = function( id, context ) {
			if ( typeof context.getElementById !== "undefined" && documentIsHTML ) {
				var elem = context.getElementById( id );
				return elem ? [ elem ] : [];
			}
		};
	} else {
		Expr.filter[ "ID" ] =  function( id ) {
			var attrId = id.replace( runescape, funescape );
			return function( elem ) {
				var node = typeof elem.getAttributeNode !== "undefined" &&
					elem.getAttributeNode( "id" );
				return node && node.value === attrId;
			};
		};

		// Support: IE 6 - 7 only
		// getElementById is not reliable as a find shortcut
		Expr.find[ "ID" ] = function( id, context ) {
			if ( typeof context.getElementById !== "undefined" && documentIsHTML ) {
				var node, i, elems,
					elem = context.getElementById( id );

				if ( elem ) {

					// Verify the id attribute
					node = elem.getAttributeNode( "id" );
					if ( node && node.value === id ) {
						return [ elem ];
					}

					// Fall back on getElementsByName
					elems = context.getElementsByName( id );
					i = 0;
					while ( ( elem = elems[ i++ ] ) ) {
						node = elem.getAttributeNode( "id" );
						if ( node && node.value === id ) {
							return [ elem ];
						}
					}
				}

				return [];
			}
		};
	}

	// Tag
	Expr.find[ "TAG" ] = support.getElementsByTagName ?
		function( tag, context ) {
			if ( typeof context.getElementsByTagName !== "undefined" ) {
				return context.getElementsByTagName( tag );

			// DocumentFragment nodes don't have gEBTN
			} else if ( support.qsa ) {
				return context.querySelectorAll( tag );
			}
		} :

		function( tag, context ) {
			var elem,
				tmp = [],
				i = 0,

				// By happy coincidence, a (broken) gEBTN appears on DocumentFragment nodes too
				results = context.getElementsByTagName( tag );

			// Filter out possible comments
			if ( tag === "*" ) {
				while ( ( elem = results[ i++ ] ) ) {
					if ( elem.nodeType === 1 ) {
						tmp.push( elem );
					}
				}

				return tmp;
			}
			return results;
		};

	// Class
	Expr.find[ "CLASS" ] = support.getElementsByClassName && function( className, context ) {
		if ( typeof context.getElementsByClassName !== "undefined" && documentIsHTML ) {
			return context.getElementsByClassName( className );
		}
	};

	/* QSA/matchesSelector
	---------------------------------------------------------------------- */

	// QSA and matchesSelector support

	// matchesSelector(:active) reports false when true (IE9/Opera 11.5)
	rbuggyMatches = [];

	// qSa(:focus) reports false when true (Chrome 21)
	// We allow this because of a bug in IE8/9 that throws an error
	// whenever `document.activeElement` is accessed on an iframe
	// So, we allow :focus to pass through QSA all the time to avoid the IE error
	// See https://bugs.jquery.com/ticket/13378
	rbuggyQSA = [];

	if ( ( support.qsa = rnative.test( document.querySelectorAll ) ) ) {

		// Build QSA regex
		// Regex strategy adopted from Diego Perini
		assert( function( el ) {

			var input;

			// Select is set to empty string on purpose
			// This is to test IE's treatment of not explicitly
			// setting a boolean content attribute,
			// since its presence should be enough
			// https://bugs.jquery.com/ticket/12359
			docElem.appendChild( el ).innerHTML = "<a id='" + expando + "'></a>" +
				"<select id='" + expando + "-\r\\' msallowcapture=''>" +
				"<option selected=''></option></select>";

			// Support: IE8, Opera 11-12.16
			// Nothing should be selected when empty strings follow ^= or $= or *=
			// The test attribute must be unknown in Opera but "safe" for WinRT
			// https://msdn.microsoft.com/en-us/library/ie/hh465388.aspx#attribute_section
			if ( el.querySelectorAll( "[msallowcapture^='']" ).length ) {
				rbuggyQSA.push( "[*^$]=" + whitespace + "*(?:''|\"\")" );
			}

			// Support: IE8
			// Boolean attributes and "value" are not treated correctly
			if ( !el.querySelectorAll( "[selected]" ).length ) {
				rbuggyQSA.push( "\\[" + whitespace + "*(?:value|" + booleans + ")" );
			}

			// Support: Chrome<29, Android<4.4, Safari<7.0+, iOS<7.0+, PhantomJS<1.9.8+
			if ( !el.querySelectorAll( "[id~=" + expando + "-]" ).length ) {
				rbuggyQSA.push( "~=" );
			}

			// Support: IE 11+, Edge 15 - 18+
			// IE 11/Edge don't find elements on a `[name='']` query in some cases.
			// Adding a temporary attribute to the document before the selection works
			// around the issue.
			// Interestingly, IE 10 & older don't seem to have the issue.
			input = document.createElement( "input" );
			input.setAttribute( "name", "" );
			el.appendChild( input );
			if ( !el.querySelectorAll( "[name='']" ).length ) {
				rbuggyQSA.push( "\\[" + whitespace + "*name" + whitespace + "*=" +
					whitespace + "*(?:''|\"\")" );
			}

			// Webkit/Opera - :checked should return selected option elements
			// http://www.w3.org/TR/2011/REC-css3-selectors-20110929/#checked
			// IE8 throws error here and will not see later tests
			if ( !el.querySelectorAll( ":checked" ).length ) {
				rbuggyQSA.push( ":checked" );
			}

			// Support: Safari 8+, iOS 8+
			// https://bugs.webkit.org/show_bug.cgi?id=136851
			// In-page `selector#id sibling-combinator selector` fails
			if ( !el.querySelectorAll( "a#" + expando + "+*" ).length ) {
				rbuggyQSA.push( ".#.+[+~]" );
			}

			// Support: Firefox <=3.6 - 5 only
			// Old Firefox doesn't throw on a badly-escaped identifier.
			el.querySelectorAll( "\\\f" );
			rbuggyQSA.push( "[\\r\\n\\f]" );
		} );

		assert( function( el ) {
			el.innerHTML = "<a href='' disabled='disabled'></a>" +
				"<select disabled='disabled'><option/></select>";

			// Support: Windows 8 Native Apps
			// The type and name attributes are restricted during .innerHTML assignment
			var input = document.createElement( "input" );
			input.setAttribute( "type", "hidden" );
			el.appendChild( input ).setAttribute( "name", "D" );

			// Support: IE8
			// Enforce case-sensitivity of name attribute
			if ( el.querySelectorAll( "[name=d]" ).length ) {
				rbuggyQSA.push( "name" + whitespace + "*[*^$|!~]?=" );
			}

			// FF 3.5 - :enabled/:disabled and hidden elements (hidden elements are still enabled)
			// IE8 throws error here and will not see later tests
			if ( el.querySelectorAll( ":enabled" ).length !== 2 ) {
				rbuggyQSA.push( ":enabled", ":disabled" );
			}

			// Support: IE9-11+
			// IE's :disabled selector does not pick up the children of disabled fieldsets
			docElem.appendChild( el ).disabled = true;
			if ( el.querySelectorAll( ":disabled" ).length !== 2 ) {
				rbuggyQSA.push( ":enabled", ":disabled" );
			}

			// Support: Opera 10 - 11 only
			// Opera 10-11 does not throw on post-comma invalid pseudos
			el.querySelectorAll( "*,:x" );
			rbuggyQSA.push( ",.*:" );
		} );
	}

	if ( ( support.matchesSelector = rnative.test( ( matches = docElem.matches ||
		docElem.webkitMatchesSelector ||
		docElem.mozMatchesSelector ||
		docElem.oMatchesSelector ||
		docElem.msMatchesSelector ) ) ) ) {

		assert( function( el ) {

			// Check to see if it's possible to do matchesSelector
			// on a disconnected node (IE 9)
			support.disconnectedMatch = matches.call( el, "*" );

			// This should fail with an exception
			// Gecko does not error, returns false instead
			matches.call( el, "[s!='']:x" );
			rbuggyMatches.push( "!=", pseudos );
		} );
	}

	rbuggyQSA = rbuggyQSA.length && new RegExp( rbuggyQSA.join( "|" ) );
	rbuggyMatches = rbuggyMatches.length && new RegExp( rbuggyMatches.join( "|" ) );

	/* Contains
	---------------------------------------------------------------------- */
	hasCompare = rnative.test( docElem.compareDocumentPosition );

	// Element contains another
	// Purposefully self-exclusive
	// As in, an element does not contain itself
	contains = hasCompare || rnative.test( docElem.contains ) ?
		function( a, b ) {
			var adown = a.nodeType === 9 ? a.documentElement : a,
				bup = b && b.parentNode;
			return a === bup || !!( bup && bup.nodeType === 1 && (
				adown.contains ?
					adown.contains( bup ) :
					a.compareDocumentPosition && a.compareDocumentPosition( bup ) & 16
			) );
		} :
		function( a, b ) {
			if ( b ) {
				while ( ( b = b.parentNode ) ) {
					if ( b === a ) {
						return true;
					}
				}
			}
			return false;
		};

	/* Sorting
	---------------------------------------------------------------------- */

	// Document order sorting
	sortOrder = hasCompare ?
	function( a, b ) {

		// Flag for duplicate removal
		if ( a === b ) {
			hasDuplicate = true;
			return 0;
		}

		// Sort on method existence if only one input has compareDocumentPosition
		var compare = !a.compareDocumentPosition - !b.compareDocumentPosition;
		if ( compare ) {
			return compare;
		}

		// Calculate position if both inputs belong to the same document
		// Support: IE 11+, Edge 17 - 18+
		// IE/Edge sometimes throw a "Permission denied" error when strict-comparing
		// two documents; shallow comparisons work.
		// eslint-disable-next-line eqeqeq
		compare = ( a.ownerDocument || a ) == ( b.ownerDocument || b ) ?
			a.compareDocumentPosition( b ) :

			// Otherwise we know they are disconnected
			1;

		// Disconnected nodes
		if ( compare & 1 ||
			( !support.sortDetached && b.compareDocumentPosition( a ) === compare ) ) {

			// Choose the first element that is related to our preferred document
			// Support: IE 11+, Edge 17 - 18+
			// IE/Edge sometimes throw a "Permission denied" error when strict-comparing
			// two documents; shallow comparisons work.
			// eslint-disable-next-line eqeqeq
			if ( a == document || a.ownerDocument == preferredDoc &&
				contains( preferredDoc, a ) ) {
				return -1;
			}

			// Support: IE 11+, Edge 17 - 18+
			// IE/Edge sometimes throw a "Permission denied" error when strict-comparing
			// two documents; shallow comparisons work.
			// eslint-disable-next-line eqeqeq
			if ( b == document || b.ownerDocument == preferredDoc &&
				contains( preferredDoc, b ) ) {
				return 1;
			}

			// Maintain original order
			return sortInput ?
				( indexOf( sortInput, a ) - indexOf( sortInput, b ) ) :
				0;
		}

		return compare & 4 ? -1 : 1;
	} :
	function( a, b ) {

		// Exit early if the nodes are identical
		if ( a === b ) {
			hasDuplicate = true;
			return 0;
		}

		var cur,
			i = 0,
			aup = a.parentNode,
			bup = b.parentNode,
			ap = [ a ],
			bp = [ b ];

		// Parentless nodes are either documents or disconnected
		if ( !aup || !bup ) {

			// Support: IE 11+, Edge 17 - 18+
			// IE/Edge sometimes throw a "Permission denied" error when strict-comparing
			// two documents; shallow comparisons work.
			/* eslint-disable eqeqeq */
			return a == document ? -1 :
				b == document ? 1 :
				/* eslint-enable eqeqeq */
				aup ? -1 :
				bup ? 1 :
				sortInput ?
				( indexOf( sortInput, a ) - indexOf( sortInput, b ) ) :
				0;

		// If the nodes are siblings, we can do a quick check
		} else if ( aup === bup ) {
			return siblingCheck( a, b );
		}

		// Otherwise we need full lists of their ancestors for comparison
		cur = a;
		while ( ( cur = cur.parentNode ) ) {
			ap.unshift( cur );
		}
		cur = b;
		while ( ( cur = cur.parentNode ) ) {
			bp.unshift( cur );
		}

		// Walk down the tree looking for a discrepancy
		while ( ap[ i ] === bp[ i ] ) {
			i++;
		}

		return i ?

			// Do a sibling check if the nodes have a common ancestor
			siblingCheck( ap[ i ], bp[ i ] ) :

			// Otherwise nodes in our document sort first
			// Support: IE 11+, Edge 17 - 18+
			// IE/Edge sometimes throw a "Permission denied" error when strict-comparing
			// two documents; shallow comparisons work.
			/* eslint-disable eqeqeq */
			ap[ i ] == preferredDoc ? -1 :
			bp[ i ] == preferredDoc ? 1 :
			/* eslint-enable eqeqeq */
			0;
	};

	return document;
};

Sizzle.matches = function( expr, elements ) {
	return Sizzle( expr, null, null, elements );
};

Sizzle.matchesSelector = function( elem, expr ) {
	setDocument( elem );

	if ( support.matchesSelector && documentIsHTML &&
		!nonnativeSelectorCache[ expr + " " ] &&
		( !rbuggyMatches || !rbuggyMatches.test( expr ) ) &&
		( !rbuggyQSA     || !rbuggyQSA.test( expr ) ) ) {

		try {
			var ret = matches.call( elem, expr );

			// IE 9's matchesSelector returns false on disconnected nodes
			if ( ret || support.disconnectedMatch ||

				// As well, disconnected nodes are said to be in a document
				// fragment in IE 9
				elem.document && elem.document.nodeType !== 11 ) {
				return ret;
			}
		} catch ( e ) {
			nonnativeSelectorCache( expr, true );
		}
	}

	return Sizzle( expr, document, null, [ elem ] ).length > 0;
};

Sizzle.contains = function( context, elem ) {

	// Set document vars if needed
	// Support: IE 11+, Edge 17 - 18+
	// IE/Edge sometimes throw a "Permission denied" error when strict-comparing
	// two documents; shallow comparisons work.
	// eslint-disable-next-line eqeqeq
	if ( ( context.ownerDocument || context ) != document ) {
		setDocument( context );
	}
	return contains( context, elem );
};

Sizzle.attr = function( elem, name ) {

	// Set document vars if needed
	// Support: IE 11+, Edge 17 - 18+
	// IE/Edge sometimes throw a "Permission denied" error when strict-comparing
	// two documents; shallow comparisons work.
	// eslint-disable-next-line eqeqeq
	if ( ( elem.ownerDocument || elem ) != document ) {
		setDocument( elem );
	}

	var fn = Expr.attrHandle[ name.toLowerCase() ],

		// Don't get fooled by Object.prototype properties (jQuery #13807)
		val = fn && hasOwn.call( Expr.attrHandle, name.toLowerCase() ) ?
			fn( elem, name, !documentIsHTML ) :
			undefined;

	return val !== undefined ?
		val :
		support.attributes || !documentIsHTML ?
			elem.getAttribute( name ) :
			( val = elem.getAttributeNode( name ) ) && val.specified ?
				val.value :
				null;
};

Sizzle.escape = function( sel ) {
	return ( sel + "" ).replace( rcssescape, fcssescape );
};

Sizzle.error = function( msg ) {
	throw new Error( "Syntax error, unrecognized expression: " + msg );
};

/**
 * Document sorting and removing duplicates
 * @param {ArrayLike} results
 */
Sizzle.uniqueSort = function( results ) {
	var elem,
		duplicates = [],
		j = 0,
		i = 0;

	// Unless we *know* we can detect duplicates, assume their presence
	hasDuplicate = !support.detectDuplicates;
	sortInput = !support.sortStable && results.slice( 0 );
	results.sort( sortOrder );

	if ( hasDuplicate ) {
		while ( ( elem = results[ i++ ] ) ) {
			if ( elem === results[ i ] ) {
				j = duplicates.push( i );
			}
		}
		while ( j-- ) {
			results.splice( duplicates[ j ], 1 );
		}
	}

	// Clear input after sorting to release objects
	// See https://github.com/jquery/sizzle/pull/225
	sortInput = null;

	return results;
};

/**
 * Utility function for retrieving the text value of an array of DOM nodes
 * @param {Array|Element} elem
 */
getText = Sizzle.getText = function( elem ) {
	var node,
		ret = "",
		i = 0,
		nodeType = elem.nodeType;

	if ( !nodeType ) {

		// If no nodeType, this is expected to be an array
		while ( ( node = elem[ i++ ] ) ) {

			// Do not traverse comment nodes
			ret += getText( node );
		}
	} else if ( nodeType === 1 || nodeType === 9 || nodeType === 11 ) {

		// Use textContent for elements
		// innerText usage removed for consistency of new lines (jQuery #11153)
		if ( typeof elem.textContent === "string" ) {
			return elem.textContent;
		} else {

			// Traverse its children
			for ( elem = elem.firstChild; elem; elem = elem.nextSibling ) {
				ret += getText( elem );
			}
		}
	} else if ( nodeType === 3 || nodeType === 4 ) {
		return elem.nodeValue;
	}

	// Do not include comment or processing instruction nodes

	return ret;
};

Expr = Sizzle.selectors = {

	// Can be adjusted by the user
	cacheLength: 50,

	createPseudo: markFunction,

	match: matchExpr,

	attrHandle: {},

	find: {},

	relative: {
		">": { dir: "parentNode", first: true },
		" ": { dir: "parentNode" },
		"+": { dir: "previousSibling", first: true },
		"~": { dir: "previousSibling" }
	},

	preFilter: {
		"ATTR": function( match ) {
			match[ 1 ] = match[ 1 ].replace( runescape, funescape );

			// Move the given value to match[3] whether quoted or unquoted
			match[ 3 ] = ( match[ 3 ] || match[ 4 ] ||
				match[ 5 ] || "" ).replace( runescape, funescape );

			if ( match[ 2 ] === "~=" ) {
				match[ 3 ] = " " + match[ 3 ] + " ";
			}

			return match.slice( 0, 4 );
		},

		"CHILD": function( match ) {

			/* matches from matchExpr["CHILD"]
				1 type (only|nth|...)
				2 what (child|of-type)
				3 argument (even|odd|\d*|\d*n([+-]\d+)?|...)
				4 xn-component of xn+y argument ([+-]?\d*n|)
				5 sign of xn-component
				6 x of xn-component
				7 sign of y-component
				8 y of y-component
			*/
			match[ 1 ] = match[ 1 ].toLowerCase();

			if ( match[ 1 ].slice( 0, 3 ) === "nth" ) {

				// nth-* requires argument
				if ( !match[ 3 ] ) {
					Sizzle.error( match[ 0 ] );
				}

				// numeric x and y parameters for Expr.filter.CHILD
				// remember that false/true cast respectively to 0/1
				match[ 4 ] = +( match[ 4 ] ?
					match[ 5 ] + ( match[ 6 ] || 1 ) :
					2 * ( match[ 3 ] === "even" || match[ 3 ] === "odd" ) );
				match[ 5 ] = +( ( match[ 7 ] + match[ 8 ] ) || match[ 3 ] === "odd" );

				// other types prohibit arguments
			} else if ( match[ 3 ] ) {
				Sizzle.error( match[ 0 ] );
			}

			return match;
		},

		"PSEUDO": function( match ) {
			var excess,
				unquoted = !match[ 6 ] && match[ 2 ];

			if ( matchExpr[ "CHILD" ].test( match[ 0 ] ) ) {
				return null;
			}

			// Accept quoted arguments as-is
			if ( match[ 3 ] ) {
				match[ 2 ] = match[ 4 ] || match[ 5 ] || "";

			// Strip excess characters from unquoted arguments
			} else if ( unquoted && rpseudo.test( unquoted ) &&

				// Get excess from tokenize (recursively)
				( excess = tokenize( unquoted, true ) ) &&

				// advance to the next closing parenthesis
				( excess = unquoted.indexOf( ")", unquoted.length - excess ) - unquoted.length ) ) {

				// excess is a negative index
				match[ 0 ] = match[ 0 ].slice( 0, excess );
				match[ 2 ] = unquoted.slice( 0, excess );
			}

			// Return only captures needed by the pseudo filter method (type and argument)
			return match.slice( 0, 3 );
		}
	},

	filter: {

		"TAG": function( nodeNameSelector ) {
			var nodeName = nodeNameSelector.replace( runescape, funescape ).toLowerCase();
			return nodeNameSelector === "*" ?
				function() {
					return true;
				} :
				function( elem ) {
					return elem.nodeName && elem.nodeName.toLowerCase() === nodeName;
				};
		},

		"CLASS": function( className ) {
			var pattern = classCache[ className + " " ];

			return pattern ||
				( pattern = new RegExp( "(^|" + whitespace +
					")" + className + "(" + whitespace + "|$)" ) ) && classCache(
						className, function( elem ) {
							return pattern.test(
								typeof elem.className === "string" && elem.className ||
								typeof elem.getAttribute !== "undefined" &&
									elem.getAttribute( "class" ) ||
								""
							);
				} );
		},

		"ATTR": function( name, operator, check ) {
			return function( elem ) {
				var result = Sizzle.attr( elem, name );

				if ( result == null ) {
					return operator === "!=";
				}
				if ( !operator ) {
					return true;
				}

				result += "";

				/* eslint-disable max-len */

				return operator === "=" ? result === check :
					operator === "!=" ? result !== check :
					operator === "^=" ? check && result.indexOf( check ) === 0 :
					operator === "*=" ? check && result.indexOf( check ) > -1 :
					operator === "$=" ? check && result.slice( -check.length ) === check :
					operator === "~=" ? ( " " + result.replace( rwhitespace, " " ) + " " ).indexOf( check ) > -1 :
					operator === "|=" ? result === check || result.slice( 0, check.length + 1 ) === check + "-" :
					false;
				/* eslint-enable max-len */

			};
		},

		"CHILD": function( type, what, _argument, first, last ) {
			var simple = type.slice( 0, 3 ) !== "nth",
				forward = type.slice( -4 ) !== "last",
				ofType = what === "of-type";

			return first === 1 && last === 0 ?

				// Shortcut for :nth-*(n)
				function( elem ) {
					return !!elem.parentNode;
				} :

				function( elem, _context, xml ) {
					var cache, uniqueCache, outerCache, node, nodeIndex, start,
						dir = simple !== forward ? "nextSibling" : "previousSibling",
						parent = elem.parentNode,
						name = ofType && elem.nodeName.toLowerCase(),
						useCache = !xml && !ofType,
						diff = false;

					if ( parent ) {

						// :(first|last|only)-(child|of-type)
						if ( simple ) {
							while ( dir ) {
								node = elem;
								while ( ( node = node[ dir ] ) ) {
									if ( ofType ?
										node.nodeName.toLowerCase() === name :
										node.nodeType === 1 ) {

										return false;
									}
								}

								// Reverse direction for :only-* (if we haven't yet done so)
								start = dir = type === "only" && !start && "nextSibling";
							}
							return true;
						}

						start = [ forward ? parent.firstChild : parent.lastChild ];

						// non-xml :nth-child(...) stores cache data on `parent`
						if ( forward && useCache ) {

							// Seek `elem` from a previously-cached index

							// ...in a gzip-friendly way
							node = parent;
							outerCache = node[ expando ] || ( node[ expando ] = {} );

							// Support: IE <9 only
							// Defend against cloned attroperties (jQuery gh-1709)
							uniqueCache = outerCache[ node.uniqueID ] ||
								( outerCache[ node.uniqueID ] = {} );

							cache = uniqueCache[ type ] || [];
							nodeIndex = cache[ 0 ] === dirruns && cache[ 1 ];
							diff = nodeIndex && cache[ 2 ];
							node = nodeIndex && parent.childNodes[ nodeIndex ];

							while ( ( node = ++nodeIndex && node && node[ dir ] ||

								// Fallback to seeking `elem` from the start
								( diff = nodeIndex = 0 ) || start.pop() ) ) {

								// When found, cache indexes on `parent` and break
								if ( node.nodeType === 1 && ++diff && node === elem ) {
									uniqueCache[ type ] = [ dirruns, nodeIndex, diff ];
									break;
								}
							}

						} else {

							// Use previously-cached element index if available
							if ( useCache ) {

								// ...in a gzip-friendly way
								node = elem;
								outerCache = node[ expando ] || ( node[ expando ] = {} );

								// Support: IE <9 only
								// Defend against cloned attroperties (jQuery gh-1709)
								uniqueCache = outerCache[ node.uniqueID ] ||
									( outerCache[ node.uniqueID ] = {} );

								cache = uniqueCache[ type ] || [];
								nodeIndex = cache[ 0 ] === dirruns && cache[ 1 ];
								diff = nodeIndex;
							}

							// xml :nth-child(...)
							// or :nth-last-child(...) or :nth(-last)?-of-type(...)
							if ( diff === false ) {

								// Use the same loop as above to seek `elem` from the start
								while ( ( node = ++nodeIndex && node && node[ dir ] ||
									( diff = nodeIndex = 0 ) || start.pop() ) ) {

									if ( ( ofType ?
										node.nodeName.toLowerCase() === name :
										node.nodeType === 1 ) &&
										++diff ) {

										// Cache the index of each encountered element
										if ( useCache ) {
											outerCache = node[ expando ] ||
												( node[ expando ] = {} );

											// Support: IE <9 only
											// Defend against cloned attroperties (jQuery gh-1709)
											uniqueCache = outerCache[ node.uniqueID ] ||
												( outerCache[ node.uniqueID ] = {} );

											uniqueCache[ type ] = [ dirruns, diff ];
										}

										if ( node === elem ) {
											break;
										}
									}
								}
							}
						}

						// Incorporate the offset, then check against cycle size
						diff -= last;
						return diff === first || ( diff % first === 0 && diff / first >= 0 );
					}
				};
		},

		"PSEUDO": function( pseudo, argument ) {

			// pseudo-class names are case-insensitive
			// http://www.w3.org/TR/selectors/#pseudo-classes
			// Prioritize by case sensitivity in case custom pseudos are added with uppercase letters
			// Remember that setFilters inherits from pseudos
			var args,
				fn = Expr.pseudos[ pseudo ] || Expr.setFilters[ pseudo.toLowerCase() ] ||
					Sizzle.error( "unsupported pseudo: " + pseudo );

			// The user may use createPseudo to indicate that
			// arguments are needed to create the filter function
			// just as Sizzle does
			if ( fn[ expando ] ) {
				return fn( argument );
			}

			// But maintain support for old signatures
			if ( fn.length > 1 ) {
				args = [ pseudo, pseudo, "", argument ];
				return Expr.setFilters.hasOwnProperty( pseudo.toLowerCase() ) ?
					markFunction( function( seed, matches ) {
						var idx,
							matched = fn( seed, argument ),
							i = matched.length;
						while ( i-- ) {
							idx = indexOf( seed, matched[ i ] );
							seed[ idx ] = !( matches[ idx ] = matched[ i ] );
						}
					} ) :
					function( elem ) {
						return fn( elem, 0, args );
					};
			}

			return fn;
		}
	},

	pseudos: {

		// Potentially complex pseudos
		"not": markFunction( function( selector ) {

			// Trim the selector passed to compile
			// to avoid treating leading and trailing
			// spaces as combinators
			var input = [],
				results = [],
				matcher = compile( selector.replace( rtrim, "$1" ) );

			return matcher[ expando ] ?
				markFunction( function( seed, matches, _context, xml ) {
					var elem,
						unmatched = matcher( seed, null, xml, [] ),
						i = seed.length;

					// Match elements unmatched by `matcher`
					while ( i-- ) {
						if ( ( elem = unmatched[ i ] ) ) {
							seed[ i ] = !( matches[ i ] = elem );
						}
					}
				} ) :
				function( elem, _context, xml ) {
					input[ 0 ] = elem;
					matcher( input, null, xml, results );

					// Don't keep the element (issue #299)
					input[ 0 ] = null;
					return !results.pop();
				};
		} ),

		"has": markFunction( function( selector ) {
			return function( elem ) {
				return Sizzle( selector, elem ).length > 0;
			};
		} ),

		"contains": markFunction( function( text ) {
			text = text.replace( runescape, funescape );
			return function( elem ) {
				return ( elem.textContent || getText( elem ) ).indexOf( text ) > -1;
			};
		} ),

		// "Whether an element is represented by a :lang() selector
		// is based solely on the element's language value
		// being equal to the identifier C,
		// or beginning with the identifier C immediately followed by "-".
		// The matching of C against the element's language value is performed case-insensitively.
		// The identifier C does not have to be a valid language name."
		// http://www.w3.org/TR/selectors/#lang-pseudo
		"lang": markFunction( function( lang ) {

			// lang value must be a valid identifier
			if ( !ridentifier.test( lang || "" ) ) {
				Sizzle.error( "unsupported lang: " + lang );
			}
			lang = lang.replace( runescape, funescape ).toLowerCase();
			return function( elem ) {
				var elemLang;
				do {
					if ( ( elemLang = documentIsHTML ?
						elem.lang :
						elem.getAttribute( "xml:lang" ) || elem.getAttribute( "lang" ) ) ) {

						elemLang = elemLang.toLowerCase();
						return elemLang === lang || elemLang.indexOf( lang + "-" ) === 0;
					}
				} while ( ( elem = elem.parentNode ) && elem.nodeType === 1 );
				return false;
			};
		} ),

		// Miscellaneous
		"target": function( elem ) {
			var hash = window.location && window.location.hash;
			return hash && hash.slice( 1 ) === elem.id;
		},

		"root": function( elem ) {
			return elem === docElem;
		},

		"focus": function( elem ) {
			return elem === document.activeElement &&
				( !document.hasFocus || document.hasFocus() ) &&
				!!( elem.type || elem.href || ~elem.tabIndex );
		},

		// Boolean properties
		"enabled": createDisabledPseudo( false ),
		"disabled": createDisabledPseudo( true ),

		"checked": function( elem ) {

			// In CSS3, :checked should return both checked and selected elements
			// http://www.w3.org/TR/2011/REC-css3-selectors-20110929/#checked
			var nodeName = elem.nodeName.toLowerCase();
			return ( nodeName === "input" && !!elem.checked ) ||
				( nodeName === "option" && !!elem.selected );
		},

		"selected": function( elem ) {

			// Accessing this property makes selected-by-default
			// options in Safari work properly
			if ( elem.parentNode ) {
				// eslint-disable-next-line no-unused-expressions
				elem.parentNode.selectedIndex;
			}

			return elem.selected === true;
		},

		// Contents
		"empty": function( elem ) {

			// http://www.w3.org/TR/selectors/#empty-pseudo
			// :empty is negated by element (1) or content nodes (text: 3; cdata: 4; entity ref: 5),
			//   but not by others (comment: 8; processing instruction: 7; etc.)
			// nodeType < 6 works because attributes (2) do not appear as children
			for ( elem = elem.firstChild; elem; elem = elem.nextSibling ) {
				if ( elem.nodeType < 6 ) {
					return false;
				}
			}
			return true;
		},

		"parent": function( elem ) {
			return !Expr.pseudos[ "empty" ]( elem );
		},

		// Element/input types
		"header": function( elem ) {
			return rheader.test( elem.nodeName );
		},

		"input": function( elem ) {
			return rinputs.test( elem.nodeName );
		},

		"button": function( elem ) {
			var name = elem.nodeName.toLowerCase();
			return name === "input" && elem.type === "button" || name === "button";
		},

		"text": function( elem ) {
			var attr;
			return elem.nodeName.toLowerCase() === "input" &&
				elem.type === "text" &&

				// Support: IE<8
				// New HTML5 attribute values (e.g., "search") appear with elem.type === "text"
				( ( attr = elem.getAttribute( "type" ) ) == null ||
					attr.toLowerCase() === "text" );
		},

		// Position-in-collection
		"first": createPositionalPseudo( function() {
			return [ 0 ];
		} ),

		"last": createPositionalPseudo( function( _matchIndexes, length ) {
			return [ length - 1 ];
		} ),

		"eq": createPositionalPseudo( function( _matchIndexes, length, argument ) {
			return [ argument < 0 ? argument + length : argument ];
		} ),

		"even": createPositionalPseudo( function( matchIndexes, length ) {
			var i = 0;
			for ( ; i < length; i += 2 ) {
				matchIndexes.push( i );
			}
			return matchIndexes;
		} ),

		"odd": createPositionalPseudo( function( matchIndexes, length ) {
			var i = 1;
			for ( ; i < length; i += 2 ) {
				matchIndexes.push( i );
			}
			return matchIndexes;
		} ),

		"lt": createPositionalPseudo( function( matchIndexes, length, argument ) {
			var i = argument < 0 ?
				argument + length :
				argument > length ?
					length :
					argument;
			for ( ; --i >= 0; ) {
				matchIndexes.push( i );
			}
			return matchIndexes;
		} ),

		"gt": createPositionalPseudo( function( matchIndexes, length, argument ) {
			var i = argument < 0 ? argument + length : argument;
			for ( ; ++i < length; ) {
				matchIndexes.push( i );
			}
			return matchIndexes;
		} )
	}
};

Expr.pseudos[ "nth" ] = Expr.pseudos[ "eq" ];

// Add button/input type pseudos
for ( i in { radio: true, checkbox: true, file: true, password: true, image: true } ) {
	Expr.pseudos[ i ] = createInputPseudo( i );
}
for ( i in { submit: true, reset: true } ) {
	Expr.pseudos[ i ] = createButtonPseudo( i );
}

// Easy API for creating new setFilters
function setFilters() {}
setFilters.prototype = Expr.filters = Expr.pseudos;
Expr.setFilters = new setFilters();

tokenize = Sizzle.tokenize = function( selector, parseOnly ) {
	var matched, match, tokens, type,
		soFar, groups, preFilters,
		cached = tokenCache[ selector + " " ];

	if ( cached ) {
		return parseOnly ? 0 : cached.slice( 0 );
	}

	soFar = selector;
	groups = [];
	preFilters = Expr.preFilter;

	while ( soFar ) {

		// Comma and first run
		if ( !matched || ( match = rcomma.exec( soFar ) ) ) {
			if ( match ) {

				// Don't consume trailing commas as valid
				soFar = soFar.slice( match[ 0 ].length ) || soFar;
			}
			groups.push( ( tokens = [] ) );
		}

		matched = false;

		// Combinators
		if ( ( match = rcombinators.exec( soFar ) ) ) {
			matched = match.shift();
			tokens.push( {
				value: matched,

				// Cast descendant combinators to space
				type: match[ 0 ].replace( rtrim, " " )
			} );
			soFar = soFar.slice( matched.length );
		}

		// Filters
		for ( type in Expr.filter ) {
			if ( ( match = matchExpr[ type ].exec( soFar ) ) && ( !preFilters[ type ] ||
				( match = preFilters[ type ]( match ) ) ) ) {
				matched = match.shift();
				tokens.push( {
					value: matched,
					type: type,
					matches: match
				} );
				soFar = soFar.slice( matched.length );
			}
		}

		if ( !matched ) {
			break;
		}
	}

	// Return the length of the invalid excess
	// if we're just parsing
	// Otherwise, throw an error or return tokens
	return parseOnly ?
		soFar.length :
		soFar ?
			Sizzle.error( selector ) :

			// Cache the tokens
			tokenCache( selector, groups ).slice( 0 );
};

function toSelector( tokens ) {
	var i = 0,
		len = tokens.length,
		selector = "";
	for ( ; i < len; i++ ) {
		selector += tokens[ i ].value;
	}
	return selector;
}

function addCombinator( matcher, combinator, base ) {
	var dir = combinator.dir,
		skip = combinator.next,
		key = skip || dir,
		checkNonElements = base && key === "parentNode",
		doneName = done++;

	return combinator.first ?

		// Check against closest ancestor/preceding element
		function( elem, context, xml ) {
			while ( ( elem = elem[ dir ] ) ) {
				if ( elem.nodeType === 1 || checkNonElements ) {
					return matcher( elem, context, xml );
				}
			}
			return false;
		} :

		// Check against all ancestor/preceding elements
		function( elem, context, xml ) {
			var oldCache, uniqueCache, outerCache,
				newCache = [ dirruns, doneName ];

			// We can't set arbitrary data on XML nodes, so they don't benefit from combinator caching
			if ( xml ) {
				while ( ( elem = elem[ dir ] ) ) {
					if ( elem.nodeType === 1 || checkNonElements ) {
						if ( matcher( elem, context, xml ) ) {
							return true;
						}
					}
				}
			} else {
				while ( ( elem = elem[ dir ] ) ) {
					if ( elem.nodeType === 1 || checkNonElements ) {
						outerCache = elem[ expando ] || ( elem[ expando ] = {} );

						// Support: IE <9 only
						// Defend against cloned attroperties (jQuery gh-1709)
						uniqueCache = outerCache[ elem.uniqueID ] ||
							( outerCache[ elem.uniqueID ] = {} );

						if ( skip && skip === elem.nodeName.toLowerCase() ) {
							elem = elem[ dir ] || elem;
						} else if ( ( oldCache = uniqueCache[ key ] ) &&
							oldCache[ 0 ] === dirruns && oldCache[ 1 ] === doneName ) {

							// Assign to newCache so results back-propagate to previous elements
							return ( newCache[ 2 ] = oldCache[ 2 ] );
						} else {

							// Reuse newcache so results back-propagate to previous elements
							uniqueCache[ key ] = newCache;

							// A match means we're done; a fail means we have to keep checking
							if ( ( newCache[ 2 ] = matcher( elem, context, xml ) ) ) {
								return true;
							}
						}
					}
				}
			}
			return false;
		};
}

function elementMatcher( matchers ) {
	return matchers.length > 1 ?
		function( elem, context, xml ) {
			var i = matchers.length;
			while ( i-- ) {
				if ( !matchers[ i ]( elem, context, xml ) ) {
					return false;
				}
			}
			return true;
		} :
		matchers[ 0 ];
}

function multipleContexts( selector, contexts, results ) {
	var i = 0,
		len = contexts.length;
	for ( ; i < len; i++ ) {
		Sizzle( selector, contexts[ i ], results );
	}
	return results;
}

function condense( unmatched, map, filter, context, xml ) {
	var elem,
		newUnmatched = [],
		i = 0,
		len = unmatched.length,
		mapped = map != null;

	for ( ; i < len; i++ ) {
		if ( ( elem = unmatched[ i ] ) ) {
			if ( !filter || filter( elem, context, xml ) ) {
				newUnmatched.push( elem );
				if ( mapped ) {
					map.push( i );
				}
			}
		}
	}

	return newUnmatched;
}

function setMatcher( preFilter, selector, matcher, postFilter, postFinder, postSelector ) {
	if ( postFilter && !postFilter[ expando ] ) {
		postFilter = setMatcher( postFilter );
	}
	if ( postFinder && !postFinder[ expando ] ) {
		postFinder = setMatcher( postFinder, postSelector );
	}
	return markFunction( function( seed, results, context, xml ) {
		var temp, i, elem,
			preMap = [],
			postMap = [],
			preexisting = results.length,

			// Get initial elements from seed or context
			elems = seed || multipleContexts(
				selector || "*",
				context.nodeType ? [ context ] : context,
				[]
			),

			// Prefilter to get matcher input, preserving a map for seed-results synchronization
			matcherIn = preFilter && ( seed || !selector ) ?
				condense( elems, preMap, preFilter, context, xml ) :
				elems,

			matcherOut = matcher ?

				// If we have a postFinder, or filtered seed, or non-seed postFilter or preexisting results,
				postFinder || ( seed ? preFilter : preexisting || postFilter ) ?

					// ...intermediate processing is necessary
					[] :

					// ...otherwise use results directly
					results :
				matcherIn;

		// Find primary matches
		if ( matcher ) {
			matcher( matcherIn, matcherOut, context, xml );
		}

		// Apply postFilter
		if ( postFilter ) {
			temp = condense( matcherOut, postMap );
			postFilter( temp, [], context, xml );

			// Un-match failing elements by moving them back to matcherIn
			i = temp.length;
			while ( i-- ) {
				if ( ( elem = temp[ i ] ) ) {
					matcherOut[ postMap[ i ] ] = !( matcherIn[ postMap[ i ] ] = elem );
				}
			}
		}

		if ( seed ) {
			if ( postFinder || preFilter ) {
				if ( postFinder ) {

					// Get the final matcherOut by condensing this intermediate into postFinder contexts
					temp = [];
					i = matcherOut.length;
					while ( i-- ) {
						if ( ( elem = matcherOut[ i ] ) ) {

							// Restore matcherIn since elem is not yet a final match
							temp.push( ( matcherIn[ i ] = elem ) );
						}
					}
					postFinder( null, ( matcherOut = [] ), temp, xml );
				}

				// Move matched elements from seed to results to keep them synchronized
				i = matcherOut.length;
				while ( i-- ) {
					if ( ( elem = matcherOut[ i ] ) &&
						( temp = postFinder ? indexOf( seed, elem ) : preMap[ i ] ) > -1 ) {

						seed[ temp ] = !( results[ temp ] = elem );
					}
				}
			}

		// Add elements to results, through postFinder if defined
		} else {
			matcherOut = condense(
				matcherOut === results ?
					matcherOut.splice( preexisting, matcherOut.length ) :
					matcherOut
			);
			if ( postFinder ) {
				postFinder( null, results, matcherOut, xml );
			} else {
				push.apply( results, matcherOut );
			}
		}
	} );
}

function matcherFromTokens( tokens ) {
	var checkContext, matcher, j,
		len = tokens.length,
		leadingRelative = Expr.relative[ tokens[ 0 ].type ],
		implicitRelative = leadingRelative || Expr.relative[ " " ],
		i = leadingRelative ? 1 : 0,

		// The foundational matcher ensures that elements are reachable from top-level context(s)
		matchContext = addCombinator( function( elem ) {
			return elem === checkContext;
		}, implicitRelative, true ),
		matchAnyContext = addCombinator( function( elem ) {
			return indexOf( checkContext, elem ) > -1;
		}, implicitRelative, true ),
		matchers = [ function( elem, context, xml ) {
			var ret = ( !leadingRelative && ( xml || context !== outermostContext ) ) || (
				( checkContext = context ).nodeType ?
					matchContext( elem, context, xml ) :
					matchAnyContext( elem, context, xml ) );

			// Avoid hanging onto element (issue #299)
			checkContext = null;
			return ret;
		} ];

	for ( ; i < len; i++ ) {
		if ( ( matcher = Expr.relative[ tokens[ i ].type ] ) ) {
			matchers = [ addCombinator( elementMatcher( matchers ), matcher ) ];
		} else {
			matcher = Expr.filter[ tokens[ i ].type ].apply( null, tokens[ i ].matches );

			// Return special upon seeing a positional matcher
			if ( matcher[ expando ] ) {

				// Find the next relative operator (if any) for proper handling
				j = ++i;
				for ( ; j < len; j++ ) {
					if ( Expr.relative[ tokens[ j ].type ] ) {
						break;
					}
				}
				return setMatcher(
					i > 1 && elementMatcher( matchers ),
					i > 1 && toSelector(

					// If the preceding token was a descendant combinator, insert an implicit any-element `*`
					tokens
						.slice( 0, i - 1 )
						.concat( { value: tokens[ i - 2 ].type === " " ? "*" : "" } )
					).replace( rtrim, "$1" ),
					matcher,
					i < j && matcherFromTokens( tokens.slice( i, j ) ),
					j < len && matcherFromTokens( ( tokens = tokens.slice( j ) ) ),
					j < len && toSelector( tokens )
				);
			}
			matchers.push( matcher );
		}
	}

	return elementMatcher( matchers );
}

function matcherFromGroupMatchers( elementMatchers, setMatchers ) {
	var bySet = setMatchers.length > 0,
		byElement = elementMatchers.length > 0,
		superMatcher = function( seed, context, xml, results, outermost ) {
			var elem, j, matcher,
				matchedCount = 0,
				i = "0",
				unmatched = seed && [],
				setMatched = [],
				contextBackup = outermostContext,

				// We must always have either seed elements or outermost context
				elems = seed || byElement && Expr.find[ "TAG" ]( "*", outermost ),

				// Use integer dirruns iff this is the outermost matcher
				dirrunsUnique = ( dirruns += contextBackup == null ? 1 : Math.random() || 0.1 ),
				len = elems.length;

			if ( outermost ) {

				// Support: IE 11+, Edge 17 - 18+
				// IE/Edge sometimes throw a "Permission denied" error when strict-comparing
				// two documents; shallow comparisons work.
				// eslint-disable-next-line eqeqeq
				outermostContext = context == document || context || outermost;
			}

			// Add elements passing elementMatchers directly to results
			// Support: IE<9, Safari
			// Tolerate NodeList properties (IE: "length"; Safari: <number>) matching elements by id
			for ( ; i !== len && ( elem = elems[ i ] ) != null; i++ ) {
				if ( byElement && elem ) {
					j = 0;

					// Support: IE 11+, Edge 17 - 18+
					// IE/Edge sometimes throw a "Permission denied" error when strict-comparing
					// two documents; shallow comparisons work.
					// eslint-disable-next-line eqeqeq
					if ( !context && elem.ownerDocument != document ) {
						setDocument( elem );
						xml = !documentIsHTML;
					}
					while ( ( matcher = elementMatchers[ j++ ] ) ) {
						if ( matcher( elem, context || document, xml ) ) {
							results.push( elem );
							break;
						}
					}
					if ( outermost ) {
						dirruns = dirrunsUnique;
					}
				}

				// Track unmatched elements for set filters
				if ( bySet ) {

					// They will have gone through all possible matchers
					if ( ( elem = !matcher && elem ) ) {
						matchedCount--;
					}

					// Lengthen the array for every element, matched or not
					if ( seed ) {
						unmatched.push( elem );
					}
				}
			}

			// `i` is now the count of elements visited above, and adding it to `matchedCount`
			// makes the latter nonnegative.
			matchedCount += i;

			// Apply set filters to unmatched elements
			// NOTE: This can be skipped if there are no unmatched elements (i.e., `matchedCount`
			// equals `i`), unless we didn't visit _any_ elements in the above loop because we have
			// no element matchers and no seed.
			// Incrementing an initially-string "0" `i` allows `i` to remain a string only in that
			// case, which will result in a "00" `matchedCount` that differs from `i` but is also
			// numerically zero.
			if ( bySet && i !== matchedCount ) {
				j = 0;
				while ( ( matcher = setMatchers[ j++ ] ) ) {
					matcher( unmatched, setMatched, context, xml );
				}

				if ( seed ) {

					// Reintegrate element matches to eliminate the need for sorting
					if ( matchedCount > 0 ) {
						while ( i-- ) {
							if ( !( unmatched[ i ] || setMatched[ i ] ) ) {
								setMatched[ i ] = pop.call( results );
							}
						}
					}

					// Discard index placeholder values to get only actual matches
					setMatched = condense( setMatched );
				}

				// Add matches to results
				push.apply( results, setMatched );

				// Seedless set matches succeeding multiple successful matchers stipulate sorting
				if ( outermost && !seed && setMatched.length > 0 &&
					( matchedCount + setMatchers.length ) > 1 ) {

					Sizzle.uniqueSort( results );
				}
			}

			// Override manipulation of globals by nested matchers
			if ( outermost ) {
				dirruns = dirrunsUnique;
				outermostContext = contextBackup;
			}

			return unmatched;
		};

	return bySet ?
		markFunction( superMatcher ) :
		superMatcher;
}

compile = Sizzle.compile = function( selector, match /* Internal Use Only */ ) {
	var i,
		setMatchers = [],
		elementMatchers = [],
		cached = compilerCache[ selector + " " ];

	if ( !cached ) {

		// Generate a function of recursive functions that can be used to check each element
		if ( !match ) {
			match = tokenize( selector );
		}
		i = match.length;
		while ( i-- ) {
			cached = matcherFromTokens( match[ i ] );
			if ( cached[ expando ] ) {
				setMatchers.push( cached );
			} else {
				elementMatchers.push( cached );
			}
		}

		// Cache the compiled function
		cached = compilerCache(
			selector,
			matcherFromGroupMatchers( elementMatchers, setMatchers )
		);

		// Save selector and tokenization
		cached.selector = selector;
	}
	return cached;
};

/**
 * A low-level selection function that works with Sizzle's compiled
 *  selector functions
 * @param {String|Function} selector A selector or a pre-compiled
 *  selector function built with Sizzle.compile
 * @param {Element} context
 * @param {Array} [results]
 * @param {Array} [seed] A set of elements to match against
 */
select = Sizzle.select = function( selector, context, results, seed ) {
	var i, tokens, token, type, find,
		compiled = typeof selector === "function" && selector,
		match = !seed && tokenize( ( selector = compiled.selector || selector ) );

	results = results || [];

	// Try to minimize operations if there is only one selector in the list and no seed
	// (the latter of which guarantees us context)
	if ( match.length === 1 ) {

		// Reduce context if the leading compound selector is an ID
		tokens = match[ 0 ] = match[ 0 ].slice( 0 );
		if ( tokens.length > 2 && ( token = tokens[ 0 ] ).type === "ID" &&
			context.nodeType === 9 && documentIsHTML && Expr.relative[ tokens[ 1 ].type ] ) {

			context = ( Expr.find[ "ID" ]( token.matches[ 0 ]
				.replace( runescape, funescape ), context ) || [] )[ 0 ];
			if ( !context ) {
				return results;

			// Precompiled matchers will still verify ancestry, so step up a level
			} else if ( compiled ) {
				context = context.parentNode;
			}

			selector = selector.slice( tokens.shift().value.length );
		}

		// Fetch a seed set for right-to-left matching
		i = matchExpr[ "needsContext" ].test( selector ) ? 0 : tokens.length;
		while ( i-- ) {
			token = tokens[ i ];

			// Abort if we hit a combinator
			if ( Expr.relative[ ( type = token.type ) ] ) {
				break;
			}
			if ( ( find = Expr.find[ type ] ) ) {

				// Search, expanding context for leading sibling combinators
				if ( ( seed = find(
					token.matches[ 0 ].replace( runescape, funescape ),
					rsibling.test( tokens[ 0 ].type ) && testContext( context.parentNode ) ||
						context
				) ) ) {

					// If seed is empty or no tokens remain, we can return early
					tokens.splice( i, 1 );
					selector = seed.length && toSelector( tokens );
					if ( !selector ) {
						push.apply( results, seed );
						return results;
					}

					break;
				}
			}
		}
	}

	// Compile and execute a filtering function if one is not provided
	// Provide `match` to avoid retokenization if we modified the selector above
	( compiled || compile( selector, match ) )(
		seed,
		context,
		!documentIsHTML,
		results,
		!context || rsibling.test( selector ) && testContext( context.parentNode ) || context
	);
	return results;
};

// One-time assignments

// Sort stability
support.sortStable = expando.split( "" ).sort( sortOrder ).join( "" ) === expando;

// Support: Chrome 14-35+
// Always assume duplicates if they aren't passed to the comparison function
support.detectDuplicates = !!hasDuplicate;

// Initialize against the default document
setDocument();

// Support: Webkit<537.32 - Safari 6.0.3/Chrome 25 (fixed in Chrome 27)
// Detached nodes confoundingly follow *each other*
support.sortDetached = assert( function( el ) {

	// Should return 1, but returns 4 (following)
	return el.compareDocumentPosition( document.createElement( "fieldset" ) ) & 1;
} );

// Support: IE<8
// Prevent attribute/property "interpolation"
// https://msdn.microsoft.com/en-us/library/ms536429%28VS.85%29.aspx
if ( !assert( function( el ) {
	el.innerHTML = "<a href='#'></a>";
	return el.firstChild.getAttribute( "href" ) === "#";
} ) ) {
	addHandle( "type|href|height|width", function( elem, name, isXML ) {
		if ( !isXML ) {
			return elem.getAttribute( name, name.toLowerCase() === "type" ? 1 : 2 );
		}
	} );
}

// Support: IE<9
// Use defaultValue in place of getAttribute("value")
if ( !support.attributes || !assert( function( el ) {
	el.innerHTML = "<input/>";
	el.firstChild.setAttribute( "value", "" );
	return el.firstChild.getAttribute( "value" ) === "";
} ) ) {
	addHandle( "value", function( elem, _name, isXML ) {
		if ( !isXML && elem.nodeName.toLowerCase() === "input" ) {
			return elem.defaultValue;
		}
	} );
}

// Support: IE<9
// Use getAttributeNode to fetch booleans when getAttribute lies
if ( !assert( function( el ) {
	return el.getAttribute( "disabled" ) == null;
} ) ) {
	addHandle( booleans, function( elem, name, isXML ) {
		var val;
		if ( !isXML ) {
			return elem[ name ] === true ? name.toLowerCase() :
				( val = elem.getAttributeNode( name ) ) && val.specified ?
					val.value :
					null;
		}
	} );
}

return Sizzle;

} )( window );



jQuery.find = Sizzle;
jQuery.expr = Sizzle.selectors;

// Deprecated
jQuery.expr[ ":" ] = jQuery.expr.pseudos;
jQuery.uniqueSort = jQuery.unique = Sizzle.uniqueSort;
jQuery.text = Sizzle.getText;
jQuery.isXMLDoc = Sizzle.isXML;
jQuery.contains = Sizzle.contains;
jQuery.escapeSelector = Sizzle.escape;




var dir = function( elem, dir, until ) {
	var matched = [],
		truncate = until !== undefined;

	while ( ( elem = elem[ dir ] ) && elem.nodeType !== 9 ) {
		if ( elem.nodeType === 1 ) {
			if ( truncate && jQuery( elem ).is( until ) ) {
				break;
			}
			matched.push( elem );
		}
	}
	return matched;
};


var siblings = function( n, elem ) {
	var matched = [];

	for ( ; n; n = n.nextSibling ) {
		if ( n.nodeType === 1 && n !== elem ) {
			matched.push( n );
		}
	}

	return matched;
};


var rneedsContext = jQuery.expr.match.needsContext;



function nodeName( elem, name ) {

	return elem.nodeName && elem.nodeName.toLowerCase() === name.toLowerCase();

}
var rsingleTag = ( /^<([a-z][^\/\0>:\x20\t\r\n\f]*)[\x20\t\r\n\f]*\/?>(?:<\/\1>|)$/i );



// Implement the identical functionality for filter and not
function winnow( elements, qualifier, not ) {
	if ( isFunction( qualifier ) ) {
		return jQuery.grep( elements, function( elem, i ) {
			return !!qualifier.call( elem, i, elem ) !== not;
		} );
	}

	// Single element
	if ( qualifier.nodeType ) {
		return jQuery.grep( elements, function( elem ) {
			return ( elem === qualifier ) !== not;
		} );
	}

	// Arraylike of elements (jQuery, arguments, Array)
	if ( typeof qualifier !== "string" ) {
		return jQuery.grep( elements, function( elem ) {
			return ( indexOf.call( qualifier, elem ) > -1 ) !== not;
		} );
	}

	// Filtered directly for both simple and complex selectors
	return jQuery.filter( qualifier, elements, not );
}

jQuery.filter = function( expr, elems, not ) {
	var elem = elems[ 0 ];

	if ( not ) {
		expr = ":not(" + expr + ")";
	}

	if ( elems.length === 1 && elem.nodeType === 1 ) {
		return jQuery.find.matchesSelector( elem, expr ) ? [ elem ] : [];
	}

	return jQuery.find.matches( expr, jQuery.grep( elems, function( elem ) {
		return elem.nodeType === 1;
	} ) );
};

jQuery.fn.extend( {
	find: function( selector ) {
		var i, ret,
			len = this.length,
			self = this;

		if ( typeof selector !== "string" ) {
			return this.pushStack( jQuery( selector ).filter( function() {
				for ( i = 0; i < len; i++ ) {
					if ( jQuery.contains( self[ i ], this ) ) {
						return true;
					}
				}
			} ) );
		}

		ret = this.pushStack( [] );

		for ( i = 0; i < len; i++ ) {
			jQuery.find( selector, self[ i ], ret );
		}

		return len > 1 ? jQuery.uniqueSort( ret ) : ret;
	},
	filter: function( selector ) {
		return this.pushStack( winnow( this, selector || [], false ) );
	},
	not: function( selector ) {
		return this.pushStack( winnow( this, selector || [], true ) );
	},
	is: function( selector ) {
		return !!winnow(
			this,

			// If this is a positional/relative selector, check membership in the returned set
			// so $("p:first").is("p:last") won't return true for a doc with two "p".
			typeof selector === "string" && rneedsContext.test( selector ) ?
				jQuery( selector ) :
				selector || [],
			false
		).length;
	}
} );


// Initialize a jQuery object


// A central reference to the root jQuery(document)
var rootjQuery,

	// A simple way to check for HTML strings
	// Prioritize #id over <tag> to avoid XSS via location.hash (#9521)
	// Strict HTML recognition (#11290: must start with <)
	// Shortcut simple #id case for speed
	rquickExpr = /^(?:\s*(<[\w\W]+>)[^>]*|#([\w-]+))$/,

	init = jQuery.fn.init = function( selector, context, root ) {
		var match, elem;

		// HANDLE: $(""), $(null), $(undefined), $(false)
		if ( !selector ) {
			return this;
		}

		// Method init() accepts an alternate rootjQuery
		// so migrate can support jQuery.sub (gh-2101)
		root = root || rootjQuery;

		// Handle HTML strings
		if ( typeof selector === "string" ) {
			if ( selector[ 0 ] === "<" &&
				selector[ selector.length - 1 ] === ">" &&
				selector.length >= 3 ) {

				// Assume that strings that start and end with <> are HTML and skip the regex check
				match = [ null, selector, null ];

			} else {
				match = rquickExpr.exec( selector );
			}

			// Match html or make sure no context is specified for #id
			if ( match && ( match[ 1 ] || !context ) ) {

				// HANDLE: $(html) -> $(array)
				if ( match[ 1 ] ) {
					context = context instanceof jQuery ? context[ 0 ] : context;

					// Option to run scripts is true for back-compat
					// Intentionally let the error be thrown if parseHTML is not present
					jQuery.merge( this, jQuery.parseHTML(
						match[ 1 ],
						context && context.nodeType ? context.ownerDocument || context : document,
						true
					) );

					// HANDLE: $(html, props)
					if ( rsingleTag.test( match[ 1 ] ) && jQuery.isPlainObject( context ) ) {
						for ( match in context ) {

							// Properties of context are called as methods if possible
							if ( isFunction( this[ match ] ) ) {
								this[ match ]( context[ match ] );

							// ...and otherwise set as attributes
							} else {
								this.attr( match, context[ match ] );
							}
						}
					}

					return this;

				// HANDLE: $(#id)
				} else {
					elem = document.getElementById( match[ 2 ] );

					if ( elem ) {

						// Inject the element directly into the jQuery object
						this[ 0 ] = elem;
						this.length = 1;
					}
					return this;
				}

			// HANDLE: $(expr, $(...))
			} else if ( !context || context.jquery ) {
				return ( context || root ).find( selector );

			// HANDLE: $(expr, context)
			// (which is just equivalent to: $(context).find(expr)
			} else {
				return this.constructor( context ).find( selector );
			}

		// HANDLE: $(DOMElement)
		} else if ( selector.nodeType ) {
			this[ 0 ] = selector;
			this.length = 1;
			return this;

		// HANDLE: $(function)
		// Shortcut for document ready
		} else if ( isFunction( selector ) ) {
			return root.ready !== undefined ?
				root.ready( selector ) :

				// Execute immediately if ready is not present
				selector( jQuery );
		}

		return jQuery.makeArray( selector, this );
	};

// Give the init function the jQuery prototype for later instantiation
init.prototype = jQuery.fn;

// Initialize central reference
rootjQuery = jQuery( document );


var rparentsprev = /^(?:parents|prev(?:Until|All))/,

	// Methods guaranteed to produce a unique set when starting from a unique set
	guaranteedUnique = {
		children: true,
		contents: true,
		next: true,
		prev: true
	};

jQuery.fn.extend( {
	has: function( target ) {
		var targets = jQuery( target, this ),
			l = targets.length;

		return this.filter( function() {
			var i = 0;
			for ( ; i < l; i++ ) {
				if ( jQuery.contains( this, targets[ i ] ) ) {
					return true;
				}
			}
		} );
	},

	closest: function( selectors, context ) {
		var cur,
			i = 0,
			l = this.length,
			matched = [],
			targets = typeof selectors !== "string" && jQuery( selectors );

		// Positional selectors never match, since there's no _selection_ context
		if ( !rneedsContext.test( selectors ) ) {
			for ( ; i < l; i++ ) {
				for ( cur = this[ i ]; cur && cur !== context; cur = cur.parentNode ) {

					// Always skip document fragments
					if ( cur.nodeType < 11 && ( targets ?
						targets.index( cur ) > -1 :

						// Don't pass non-elements to Sizzle
						cur.nodeType === 1 &&
							jQuery.find.matchesSelector( cur, selectors ) ) ) {

						matched.push( cur );
						break;
					}
				}
			}
		}

		return this.pushStack( matched.length > 1 ? jQuery.uniqueSort( matched ) : matched );
	},

	// Determine the position of an element within the set
	index: function( elem ) {

		// No argument, return index in parent
		if ( !elem ) {
			return ( this[ 0 ] && this[ 0 ].parentNode ) ? this.first().prevAll().length : -1;
		}

		// Index in selector
		if ( typeof elem === "string" ) {
			return indexOf.call( jQuery( elem ), this[ 0 ] );
		}

		// Locate the position of the desired element
		return indexOf.call( this,

			// If it receives a jQuery object, the first element is used
			elem.jquery ? elem[ 0 ] : elem
		);
	},

	add: function( selector, context ) {
		return this.pushStack(
			jQuery.uniqueSort(
				jQuery.merge( this.get(), jQuery( selector, context ) )
			)
		);
	},

	addBack: function( selector ) {
		return this.add( selector == null ?
			this.prevObject : this.prevObject.filter( selector )
		);
	}
} );

function sibling( cur, dir ) {
	while ( ( cur = cur[ dir ] ) && cur.nodeType !== 1 ) {}
	return cur;
}

jQuery.each( {
	parent: function( elem ) {
		var parent = elem.parentNode;
		return parent && parent.nodeType !== 11 ? parent : null;
	},
	parents: function( elem ) {
		return dir( elem, "parentNode" );
	},
	parentsUntil: function( elem, _i, until ) {
		return dir( elem, "parentNode", until );
	},
	next: function( elem ) {
		return sibling( elem, "nextSibling" );
	},
	prev: function( elem ) {
		return sibling( elem, "previousSibling" );
	},
	nextAll: function( elem ) {
		return dir( elem, "nextSibling" );
	},
	prevAll: function( elem ) {
		return dir( elem, "previousSibling" );
	},
	nextUntil: function( elem, _i, until ) {
		return dir( elem, "nextSibling", until );
	},
	prevUntil: function( elem, _i, until ) {
		return dir( elem, "previousSibling", until );
	},
	siblings: function( elem ) {
		return siblings( ( elem.parentNode || {} ).firstChild, elem );
	},
	children: function( elem ) {
		return siblings( elem.firstChild );
	},
	contents: function( elem ) {
		if ( elem.contentDocument != null &&

			// Support: IE 11+
			// <object> elements with no `data` attribute has an object
			// `contentDocument` with a `null` prototype.
			getProto( elem.contentDocument ) ) {

			return elem.contentDocument;
		}

		// Support: IE 9 - 11 only, iOS 7 only, Android Browser <=4.3 only
		// Treat the template element as a regular one in browsers that
		// don't support it.
		if ( nodeName( elem, "template" ) ) {
			elem = elem.content || elem;
		}

		return jQuery.merge( [], elem.childNodes );
	}
}, function( name, fn ) {
	jQuery.fn[ name ] = function( until, selector ) {
		var matched = jQuery.map( this, fn, until );

		if ( name.slice( -5 ) !== "Until" ) {
			selector = until;
		}

		if ( selector && typeof selector === "string" ) {
			matched = jQuery.filter( selector, matched );
		}

		if ( this.length > 1 ) {

			// Remove duplicates
			if ( !guaranteedUnique[ name ] ) {
				jQuery.uniqueSort( matched );
			}

			// Reverse order for parents* and prev-derivatives
			if ( rparentsprev.test( name ) ) {
				matched.reverse();
			}
		}

		return this.pushStack( matched );
	};
} );
var rnothtmlwhite = ( /[^\x20\t\r\n\f]+/g );



// Convert String-formatted options into Object-formatted ones
function createOptions( options ) {
	var object = {};
	jQuery.each( options.match( rnothtmlwhite ) || [], function( _, flag ) {
		object[ flag ] = true;
	} );
	return object;
}

/*
 * Create a callback list using the following parameters:
 *
 *	options: an optional list of space-separated options that will change how
 *			the callback list behaves or a more traditional option object
 *
 * By default a callback list will act like an event callback list and can be
 * "fired" multiple times.
 *
 * Possible options:
 *
 *	once:			will ensure the callback list can only be fired once (like a Deferred)
 *
 *	memory:			will keep track of previous values and will call any callback added
 *					after the list has been fired right away with the latest "memorized"
 *					values (like a Deferred)
 *
 *	unique:			will ensure a callback can only be added once (no duplicate in the list)
 *
 *	stopOnFalse:	interrupt callings when a callback returns false
 *
 */
jQuery.Callbacks = function( options ) {

	// Convert options from String-formatted to Object-formatted if needed
	// (we check in cache first)
	options = typeof options === "string" ?
		createOptions( options ) :
		jQuery.extend( {}, options );

	var // Flag to know if list is currently firing
		firing,

		// Last fire value for non-forgettable lists
		memory,

		// Flag to know if list was already fired
		fired,

		// Flag to prevent firing
		locked,

		// Actual callback list
		list = [],

		// Queue of execution data for repeatable lists
		queue = [],

		// Index of currently firing callback (modified by add/remove as needed)
		firingIndex = -1,

		// Fire callbacks
		fire = function() {

			// Enforce single-firing
			locked = locked || options.once;

			// Execute callbacks for all pending executions,
			// respecting firingIndex overrides and runtime changes
			fired = firing = true;
			for ( ; queue.length; firingIndex = -1 ) {
				memory = queue.shift();
				while ( ++firingIndex < list.length ) {

					// Run callback and check for early termination
					if ( list[ firingIndex ].apply( memory[ 0 ], memory[ 1 ] ) === false &&
						options.stopOnFalse ) {

						// Jump to end and forget the data so .add doesn't re-fire
						firingIndex = list.length;
						memory = false;
					}
				}
			}

			// Forget the data if we're done with it
			if ( !options.memory ) {
				memory = false;
			}

			firing = false;

			// Clean up if we're done firing for good
			if ( locked ) {

				// Keep an empty list if we have data for future add calls
				if ( memory ) {
					list = [];

				// Otherwise, this object is spent
				} else {
					list = "";
				}
			}
		},

		// Actual Callbacks object
		self = {

			// Add a callback or a collection of callbacks to the list
			add: function() {
				if ( list ) {

					// If we have memory from a past run, we should fire after adding
					if ( memory && !firing ) {
						firingIndex = list.length - 1;
						queue.push( memory );
					}

					( function add( args ) {
						jQuery.each( args, function( _, arg ) {
							if ( isFunction( arg ) ) {
								if ( !options.unique || !self.has( arg ) ) {
									list.push( arg );
								}
							} else if ( arg && arg.length && toType( arg ) !== "string" ) {

								// Inspect recursively
								add( arg );
							}
						} );
					} )( arguments );

					if ( memory && !firing ) {
						fire();
					}
				}
				return this;
			},

			// Remove a callback from the list
			remove: function() {
				jQuery.each( arguments, function( _, arg ) {
					var index;
					while ( ( index = jQuery.inArray( arg, list, index ) ) > -1 ) {
						list.splice( index, 1 );

						// Handle firing indexes
						if ( index <= firingIndex ) {
							firingIndex--;
						}
					}
				} );
				return this;
			},

			// Check if a given callback is in the list.
			// If no argument is given, return whether or not list has callbacks attached.
			has: function( fn ) {
				return fn ?
					jQuery.inArray( fn, list ) > -1 :
					list.length > 0;
			},

			// Remove all callbacks from the list
			empty: function() {
				if ( list ) {
					list = [];
				}
				return this;
			},

			// Disable .fire and .add
			// Abort any current/pending executions
			// Clear all callbacks and values
			disable: function() {
				locked = queue = [];
				list = memory = "";
				return this;
			},
			disabled: function() {
				return !list;
			},

			// Disable .fire
			// Also disable .add unless we have memory (since it would have no effect)
			// Abort any pending executions
			lock: function() {
				locked = queue = [];
				if ( !memory && !firing ) {
					list = memory = "";
				}
				return this;
			},
			locked: function() {
				return !!locked;
			},

			// Call all callbacks with the given context and arguments
			fireWith: function( context, args ) {
				if ( !locked ) {
					args = args || [];
					args = [ context, args.slice ? args.slice() : args ];
					queue.push( args );
					if ( !firing ) {
						fire();
					}
				}
				return this;
			},

			// Call all the callbacks with the given arguments
			fire: function() {
				self.fireWith( this, arguments );
				return this;
			},

			// To know if the callbacks have already been called at least once
			fired: function() {
				return !!fired;
			}
		};

	return self;
};


function Identity( v ) {
	return v;
}
function Thrower( ex ) {
	throw ex;
}

function adoptValue( value, resolve, reject, noValue ) {
	var method;

	try {

		// Check for promise aspect first to privilege synchronous behavior
		if ( value && isFunction( ( method = value.promise ) ) ) {
			method.call( value ).done( resolve ).fail( reject );

		// Other thenables
		} else if ( value && isFunction( ( method = value.then ) ) ) {
			method.call( value, resolve, reject );

		// Other non-thenables
		} else {

			// Control `resolve` arguments by letting Array#slice cast boolean `noValue` to integer:
			// * false: [ value ].slice( 0 ) => resolve( value )
			// * true: [ value ].slice( 1 ) => resolve()
			resolve.apply( undefined, [ value ].slice( noValue ) );
		}

	// For Promises/A+, convert exceptions into rejections
	// Since jQuery.when doesn't unwrap thenables, we can skip the extra checks appearing in
	// Deferred#then to conditionally suppress rejection.
	} catch ( value ) {

		// Support: Android 4.0 only
		// Strict mode functions invoked without .call/.apply get global-object context
		reject.apply( undefined, [ value ] );
	}
}

jQuery.extend( {

	Deferred: function( func ) {
		var tuples = [

				// action, add listener, callbacks,
				// ... .then handlers, argument index, [final state]
				[ "notify", "progress", jQuery.Callbacks( "memory" ),
					jQuery.Callbacks( "memory" ), 2 ],
				[ "resolve", "done", jQuery.Callbacks( "once memory" ),
					jQuery.Callbacks( "once memory" ), 0, "resolved" ],
				[ "reject", "fail", jQuery.Callbacks( "once memory" ),
					jQuery.Callbacks( "once memory" ), 1, "rejected" ]
			],
			state = "pending",
			promise = {
				state: function() {
					return state;
				},
				always: function() {
					deferred.done( arguments ).fail( arguments );
					return this;
				},
				"catch": function( fn ) {
					return promise.then( null, fn );
				},

				// Keep pipe for back-compat
				pipe: function( /* fnDone, fnFail, fnProgress */ ) {
					var fns = arguments;

					return jQuery.Deferred( function( newDefer ) {
						jQuery.each( tuples, function( _i, tuple ) {

							// Map tuples (progress, done, fail) to arguments (done, fail, progress)
							var fn = isFunction( fns[ tuple[ 4 ] ] ) && fns[ tuple[ 4 ] ];

							// deferred.progress(function() { bind to newDefer or newDefer.notify })
							// deferred.done(function() { bind to newDefer or newDefer.resolve })
							// deferred.fail(function() { bind to newDefer or newDefer.reject })
							deferred[ tuple[ 1 ] ]( function() {
								var returned = fn && fn.apply( this, arguments );
								if ( returned && isFunction( returned.promise ) ) {
									returned.promise()
										.progress( newDefer.notify )
										.done( newDefer.resolve )
										.fail( newDefer.reject );
								} else {
									newDefer[ tuple[ 0 ] + "With" ](
										this,
										fn ? [ returned ] : arguments
									);
								}
							} );
						} );
						fns = null;
					} ).promise();
				},
				then: function( onFulfilled, onRejected, onProgress ) {
					var maxDepth = 0;
					function resolve( depth, deferred, handler, special ) {
						return function() {
							var that = this,
								args = arguments,
								mightThrow = function() {
									var returned, then;

									// Support: Promises/A+ section 2.3.3.3.3
									// https://promisesaplus.com/#point-59
									// Ignore double-resolution attempts
									if ( depth < maxDepth ) {
										return;
									}

									returned = handler.apply( that, args );

									// Support: Promises/A+ section 2.3.1
									// https://promisesaplus.com/#point-48
									if ( returned === deferred.promise() ) {
										throw new TypeError( "Thenable self-resolution" );
									}

									// Support: Promises/A+ sections 2.3.3.1, 3.5
									// https://promisesaplus.com/#point-54
									// https://promisesaplus.com/#point-75
									// Retrieve `then` only once
									then = returned &&

										// Support: Promises/A+ section 2.3.4
										// https://promisesaplus.com/#point-64
										// Only check objects and functions for thenability
										( typeof returned === "object" ||
											typeof returned === "function" ) &&
										returned.then;

									// Handle a returned thenable
									if ( isFunction( then ) ) {

										// Special processors (notify) just wait for resolution
										if ( special ) {
											then.call(
												returned,
												resolve( maxDepth, deferred, Identity, special ),
												resolve( maxDepth, deferred, Thrower, special )
											);

										// Normal processors (resolve) also hook into progress
										} else {

											// ...and disregard older resolution values
											maxDepth++;

											then.call(
												returned,
												resolve( maxDepth, deferred, Identity, special ),
												resolve( maxDepth, deferred, Thrower, special ),
												resolve( maxDepth, deferred, Identity,
													deferred.notifyWith )
											);
										}

									// Handle all other returned values
									} else {

										// Only substitute handlers pass on context
										// and multiple values (non-spec behavior)
										if ( handler !== Identity ) {
											that = undefined;
											args = [ returned ];
										}

										// Process the value(s)
										// Default process is resolve
										( special || deferred.resolveWith )( that, args );
									}
								},

								// Only normal processors (resolve) catch and reject exceptions
								process = special ?
									mightThrow :
									function() {
										try {
											mightThrow();
										} catch ( e ) {

											if ( jQuery.Deferred.exceptionHook ) {
												jQuery.Deferred.exceptionHook( e,
													process.stackTrace );
											}

											// Support: Promises/A+ section 2.3.3.3.4.1
											// https://promisesaplus.com/#point-61
											// Ignore post-resolution exceptions
											if ( depth + 1 >= maxDepth ) {

												// Only substitute handlers pass on context
												// and multiple values (non-spec behavior)
												if ( handler !== Thrower ) {
													that = undefined;
													args = [ e ];
												}

												deferred.rejectWith( that, args );
											}
										}
									};

							// Support: Promises/A+ section 2.3.3.3.1
							// https://promisesaplus.com/#point-57
							// Re-resolve promises immediately to dodge false rejection from
							// subsequent errors
							if ( depth ) {
								process();
							} else {

								// Call an optional hook to record the stack, in case of exception
								// since it's otherwise lost when execution goes async
								if ( jQuery.Deferred.getStackHook ) {
									process.stackTrace = jQuery.Deferred.getStackHook();
								}
								window.setTimeout( process );
							}
						};
					}

					return jQuery.Deferred( function( newDefer ) {

						// progress_handlers.add( ... )
						tuples[ 0 ][ 3 ].add(
							resolve(
								0,
								newDefer,
								isFunction( onProgress ) ?
									onProgress :
									Identity,
								newDefer.notifyWith
							)
						);

						// fulfilled_handlers.add( ... )
						tuples[ 1 ][ 3 ].add(
							resolve(
								0,
								newDefer,
								isFunction( onFulfilled ) ?
									onFulfilled :
									Identity
							)
						);

						// rejected_handlers.add( ... )
						tuples[ 2 ][ 3 ].add(
							resolve(
								0,
								newDefer,
								isFunction( onRejected ) ?
									onRejected :
									Thrower
							)
						);
					} ).promise();
				},

				// Get a promise for this deferred
				// If obj is provided, the promise aspect is added to the object
				promise: function( obj ) {
					return obj != null ? jQuery.extend( obj, promise ) : promise;
				}
			},
			deferred = {};

		// Add list-specific methods
		jQuery.each( tuples, function( i, tuple ) {
			var list = tuple[ 2 ],
				stateString = tuple[ 5 ];

			// promise.progress = list.add
			// promise.done = list.add
			// promise.fail = list.add
			promise[ tuple[ 1 ] ] = list.add;

			// Handle state
			if ( stateString ) {
				list.add(
					function() {

						// state = "resolved" (i.e., fulfilled)
						// state = "rejected"
						state = stateString;
					},

					// rejected_callbacks.disable
					// fulfilled_callbacks.disable
					tuples[ 3 - i ][ 2 ].disable,

					// rejected_handlers.disable
					// fulfilled_handlers.disable
					tuples[ 3 - i ][ 3 ].disable,

					// progress_callbacks.lock
					tuples[ 0 ][ 2 ].lock,

					// progress_handlers.lock
					tuples[ 0 ][ 3 ].lock
				);
			}

			// progress_handlers.fire
			// fulfilled_handlers.fire
			// rejected_handlers.fire
			list.add( tuple[ 3 ].fire );

			// deferred.notify = function() { deferred.notifyWith(...) }
			// deferred.resolve = function() { deferred.resolveWith(...) }
			// deferred.reject = function() { deferred.rejectWith(...) }
			deferred[ tuple[ 0 ] ] = function() {
				deferred[ tuple[ 0 ] + "With" ]( this === deferred ? undefined : this, arguments );
				return this;
			};

			// deferred.notifyWith = list.fireWith
			// deferred.resolveWith = list.fireWith
			// deferred.rejectWith = list.fireWith
			deferred[ tuple[ 0 ] + "With" ] = list.fireWith;
		} );

		// Make the deferred a promise
		promise.promise( deferred );

		// Call given func if any
		if ( func ) {
			func.call( deferred, deferred );
		}

		// All done!
		return deferred;
	},

	// Deferred helper
	when: function( singleValue ) {
		var

			// count of uncompleted subordinates
			remaining = arguments.length,

			// count of unprocessed arguments
			i = remaining,

			// subordinate fulfillment data
			resolveContexts = Array( i ),
			resolveValues = slice.call( arguments ),

			// the primary Deferred
			primary = jQuery.Deferred(),

			// subordinate callback factory
			updateFunc = function( i ) {
				return function( value ) {
					resolveContexts[ i ] = this;
					resolveValues[ i ] = arguments.length > 1 ? slice.call( arguments ) : value;
					if ( !( --remaining ) ) {
						primary.resolveWith( resolveContexts, resolveValues );
					}
				};
			};

		// Single- and empty arguments are adopted like Promise.resolve
		if ( remaining <= 1 ) {
			adoptValue( singleValue, primary.done( updateFunc( i ) ).resolve, primary.reject,
				!remaining );

			// Use .then() to unwrap secondary thenables (cf. gh-3000)
			if ( primary.state() === "pending" ||
				isFunction( resolveValues[ i ] && resolveValues[ i ].then ) ) {

				return primary.then();
			}
		}

		// Multiple arguments are aggregated like Promise.all array elements
		while ( i-- ) {
			adoptValue( resolveValues[ i ], updateFunc( i ), primary.reject );
		}

		return primary.promise();
	}
} );


// These usually indicate a programmer mistake during development,
// warn about them ASAP rather than swallowing them by default.
var rerrorNames = /^(Eval|Internal|Range|Reference|Syntax|Type|URI)Error$/;

jQuery.Deferred.exceptionHook = function( error, stack ) {

	// Support: IE 8 - 9 only
	// Console exists when dev tools are open, which can happen at any time
	if ( window.console && window.console.warn && error && rerrorNames.test( error.name ) ) {
		window.console.warn( "jQuery.Deferred exception: " + error.message, error.stack, stack );
	}
};




jQuery.readyException = function( error ) {
	window.setTimeout( function() {
		throw error;
	} );
};




// The deferred used on DOM ready
var readyList = jQuery.Deferred();

jQuery.fn.ready = function( fn ) {

	readyList
		.then( fn )

		// Wrap jQuery.readyException in a function so that the lookup
		// happens at the time of error handling instead of callback
		// registration.
		.catch( function( error ) {
			jQuery.readyException( error );
		} );

	return this;
};

jQuery.extend( {

	// Is the DOM ready to be used? Set to true once it occurs.
	isReady: false,

	// A counter to track how many items to wait for before
	// the ready event fires. See #6781
	readyWait: 1,

	// Handle when the DOM is ready
	ready: function( wait ) {

		// Abort if there are pending holds or we're already ready
		if ( wait === true ? --jQuery.readyWait : jQuery.isReady ) {
			return;
		}

		// Remember that the DOM is ready
		jQuery.isReady = true;

		// If a normal DOM Ready event fired, decrement, and wait if need be
		if ( wait !== true && --jQuery.readyWait > 0 ) {
			return;
		}

		// If there are functions bound, to execute
		readyList.resolveWith( document, [ jQuery ] );
	}
} );

jQuery.ready.then = readyList.then;

// The ready event handler and self cleanup method
function completed() {
	document.removeEventListener( "DOMContentLoaded", completed );
	window.removeEventListener( "load", completed );
	jQuery.ready();
}

// Catch cases where $(document).ready() is called
// after the browser event has already occurred.
// Support: IE <=9 - 10 only
// Older IE sometimes signals "interactive" too soon
if ( document.readyState === "complete" ||
	( document.readyState !== "loading" && !document.documentElement.doScroll ) ) {

	// Handle it asynchronously to allow scripts the opportunity to delay ready
	window.setTimeout( jQuery.ready );

} else {

	// Use the handy event callback
	document.addEventListener( "DOMContentLoaded", completed );

	// A fallback to window.onload, that will always work
	window.addEventListener( "load", completed );
}




// Multifunctional method to get and set values of a collection
// The value/s can optionally be executed if it's a function
var access = function( elems, fn, key, value, chainable, emptyGet, raw ) {
	var i = 0,
		len = elems.length,
		bulk = key == null;

	// Sets many values
	if ( toType( key ) === "object" ) {
		chainable = true;
		for ( i in key ) {
			access( elems, fn, i, key[ i ], true, emptyGet, raw );
		}

	// Sets one value
	} else if ( value !== undefined ) {
		chainable = true;

		if ( !isFunction( value ) ) {
			raw = true;
		}

		if ( bulk ) {

			// Bulk operations run against the entire set
			if ( raw ) {
				fn.call( elems, value );
				fn = null;

			// ...except when executing function values
			} else {
				bulk = fn;
				fn = function( elem, _key, value ) {
					return bulk.call( jQuery( elem ), value );
				};
			}
		}

		if ( fn ) {
			for ( ; i < len; i++ ) {
				fn(
					elems[ i ], key, raw ?
						value :
						value.call( elems[ i ], i, fn( elems[ i ], key ) )
				);
			}
		}
	}

	if ( chainable ) {
		return elems;
	}

	// Gets
	if ( bulk ) {
		return fn.call( elems );
	}

	return len ? fn( elems[ 0 ], key ) : emptyGet;
};


// Matches dashed string for camelizing
var rmsPrefix = /^-ms-/,
	rdashAlpha = /-([a-z])/g;

// Used by camelCase as callback to replace()
function fcamelCase( _all, letter ) {
	return letter.toUpperCase();
}

// Convert dashed to camelCase; used by the css and data modules
// Support: IE <=9 - 11, Edge 12 - 15
// Microsoft forgot to hump their vendor prefix (#9572)
function camelCase( string ) {
	return string.replace( rmsPrefix, "ms-" ).replace( rdashAlpha, fcamelCase );
}
var acceptData = function( owner ) {

	// Accepts only:
	//  - Node
	//    - Node.ELEMENT_NODE
	//    - Node.DOCUMENT_NODE
	//  - Object
	//    - Any
	return owner.nodeType === 1 || owner.nodeType === 9 || !( +owner.nodeType );
};




function Data() {
	this.expando = jQuery.expando + Data.uid++;
}

Data.uid = 1;

Data.prototype = {

	cache: function( owner ) {

		// Check if the owner object already has a cache
		var value = owner[ this.expando ];

		// If not, create one
		if ( !value ) {
			value = {};

			// We can accept data for non-element nodes in modern browsers,
			// but we should not, see #8335.
			// Always return an empty object.
			if ( acceptData( owner ) ) {

				// If it is a node unlikely to be stringify-ed or looped over
				// use plain assignment
				if ( owner.nodeType ) {
					owner[ this.expando ] = value;

				// Otherwise secure it in a non-enumerable property
				// configurable must be true to allow the property to be
				// deleted when data is removed
				} else {
					Object.defineProperty( owner, this.expando, {
						value: value,
						configurable: true
					} );
				}
			}
		}

		return value;
	},
	set: function( owner, data, value ) {
		var prop,
			cache = this.cache( owner );

		// Handle: [ owner, key, value ] args
		// Always use camelCase key (gh-2257)
		if ( typeof data === "string" ) {
			cache[ camelCase( data ) ] = value;

		// Handle: [ owner, { properties } ] args
		} else {

			// Copy the properties one-by-one to the cache object
			for ( prop in data ) {
				cache[ camelCase( prop ) ] = data[ prop ];
			}
		}
		return cache;
	},
	get: function( owner, key ) {
		return key === undefined ?
			this.cache( owner ) :

			// Always use camelCase key (gh-2257)
			owner[ this.expando ] && owner[ this.expando ][ camelCase( key ) ];
	},
	access: function( owner, key, value ) {

		// In cases where either:
		//
		//   1. No key was specified
		//   2. A string key was specified, but no value provided
		//
		// Take the "read" path and allow the get method to determine
		// which value to return, respectively either:
		//
		//   1. The entire cache object
		//   2. The data stored at the key
		//
		if ( key === undefined ||
				( ( key && typeof key === "string" ) && value === undefined ) ) {

			return this.get( owner, key );
		}

		// When the key is not a string, or both a key and value
		// are specified, set or extend (existing objects) with either:
		//
		//   1. An object of properties
		//   2. A key and value
		//
		this.set( owner, key, value );

		// Since the "set" path can have two possible entry points
		// return the expected data based on which path was taken[*]
		return value !== undefined ? value : key;
	},
	remove: function( owner, key ) {
		var i,
			cache = owner[ this.expando ];

		if ( cache === undefined ) {
			return;
		}

		if ( key !== undefined ) {

			// Support array or space separated string of keys
			if ( Array.isArray( key ) ) {

				// If key is an array of keys...
				// We always set camelCase keys, so remove that.
				key = key.map( camelCase );
			} else {
				key = camelCase( key );

				// If a key with the spaces exists, use it.
				// Otherwise, create an array by matching non-whitespace
				key = key in cache ?
					[ key ] :
					( key.match( rnothtmlwhite ) || [] );
			}

			i = key.length;

			while ( i-- ) {
				delete cache[ key[ i ] ];
			}
		}

		// Remove the expando if there's no more data
		if ( key === undefined || jQuery.isEmptyObject( cache ) ) {

			// Support: Chrome <=35 - 45
			// Webkit & Blink performance suffers when deleting properties
			// from DOM nodes, so set to undefined instead
			// https://bugs.chromium.org/p/chromium/issues/detail?id=378607 (bug restricted)
			if ( owner.nodeType ) {
				owner[ this.expando ] = undefined;
			} else {
				delete owner[ this.expando ];
			}
		}
	},
	hasData: function( owner ) {
		var cache = owner[ this.expando ];
		return cache !== undefined && !jQuery.isEmptyObject( cache );
	}
};
var dataPriv = new Data();

var dataUser = new Data();



//	Implementation Summary
//
//	1. Enforce API surface and semantic compatibility with 1.9.x branch
//	2. Improve the module's maintainability by reducing the storage
//		paths to a single mechanism.
//	3. Use the same single mechanism to support "private" and "user" data.
//	4. _Never_ expose "private" data to user code (TODO: Drop _data, _removeData)
//	5. Avoid exposing implementation details on user objects (eg. expando properties)
//	6. Provide a clear path for implementation upgrade to WeakMap in 2014

var rbrace = /^(?:\{[\w\W]*\}|\[[\w\W]*\])$/,
	rmultiDash = /[A-Z]/g;

function getData( data ) {
	if ( data === "true" ) {
		return true;
	}

	if ( data === "false" ) {
		return false;
	}

	if ( data === "null" ) {
		return null;
	}

	// Only convert to a number if it doesn't change the string
	if ( data === +data + "" ) {
		return +data;
	}

	if ( rbrace.test( data ) ) {
		return JSON.parse( data );
	}

	return data;
}

function dataAttr( elem, key, data ) {
	var name;

	// If nothing was found internally, try to fetch any
	// data from the HTML5 data-* attribute
	if ( data === undefined && elem.nodeType === 1 ) {
		name = "data-" + key.replace( rmultiDash, "-$&" ).toLowerCase();
		data = elem.getAttribute( name );

		if ( typeof data === "string" ) {
			try {
				data = getData( data );
			} catch ( e ) {}

			// Make sure we set the data so it isn't changed later
			dataUser.set( elem, key, data );
		} else {
			data = undefined;
		}
	}
	return data;
}

jQuery.extend( {
	hasData: function( elem ) {
		return dataUser.hasData( elem ) || dataPriv.hasData( elem );
	},

	data: function( elem, name, data ) {
		return dataUser.access( elem, name, data );
	},

	removeData: function( elem, name ) {
		dataUser.remove( elem, name );
	},

	// TODO: Now that all calls to _data and _removeData have been replaced
	// with direct calls to dataPriv methods, these can be deprecated.
	_data: function( elem, name, data ) {
		return dataPriv.access( elem, name, data );
	},

	_removeData: function( elem, name ) {
		dataPriv.remove( elem, name );
	}
} );

jQuery.fn.extend( {
	data: function( key, value ) {
		var i, name, data,
			elem = this[ 0 ],
			attrs = elem && elem.attributes;

		// Gets all values
		if ( key === undefined ) {
			if ( this.length ) {
				data = dataUser.get( elem );

				if ( elem.nodeType === 1 && !dataPriv.get( elem, "hasDataAttrs" ) ) {
					i = attrs.length;
					while ( i-- ) {

						// Support: IE 11 only
						// The attrs elements can be null (#14894)
						if ( attrs[ i ] ) {
							name = attrs[ i ].name;
							if ( name.indexOf( "data-" ) === 0 ) {
								name = camelCase( name.slice( 5 ) );
								dataAttr( elem, name, data[ name ] );
							}
						}
					}
					dataPriv.set( elem, "hasDataAttrs", true );
				}
			}

			return data;
		}

		// Sets multiple values
		if ( typeof key === "object" ) {
			return this.each( function() {
				dataUser.set( this, key );
			} );
		}

		return access( this, function( value ) {
			var data;

			// The calling jQuery object (element matches) is not empty
			// (and therefore has an element appears at this[ 0 ]) and the
			// `value` parameter was not undefined. An empty jQuery object
			// will result in `undefined` for elem = this[ 0 ] which will
			// throw an exception if an attempt to read a data cache is made.
			if ( elem && value === undefined ) {

				// Attempt to get data from the cache
				// The key will always be camelCased in Data
				data = dataUser.get( elem, key );
				if ( data !== undefined ) {
					return data;
				}

				// Attempt to "discover" the data in
				// HTML5 custom data-* attrs
				data = dataAttr( elem, key );
				if ( data !== undefined ) {
					return data;
				}

				// We tried really hard, but the data doesn't exist.
				return;
			}

			// Set the data...
			this.each( function() {

				// We always store the camelCased key
				dataUser.set( this, key, value );
			} );
		}, null, value, arguments.length > 1, null, true );
	},

	removeData: function( key ) {
		return this.each( function() {
			dataUser.remove( this, key );
		} );
	}
} );


jQuery.extend( {
	queue: function( elem, type, data ) {
		var queue;

		if ( elem ) {
			type = ( type || "fx" ) + "queue";
			queue = dataPriv.get( elem, type );

			// Speed up dequeue by getting out quickly if this is just a lookup
			if ( data ) {
				if ( !queue || Array.isArray( data ) ) {
					queue = dataPriv.access( elem, type, jQuery.makeArray( data ) );
				} else {
					queue.push( data );
				}
			}
			return queue || [];
		}
	},

	dequeue: function( elem, type ) {
		type = type || "fx";

		var queue = jQuery.queue( elem, type ),
			startLength = queue.length,
			fn = queue.shift(),
			hooks = jQuery._queueHooks( elem, type ),
			next = function() {
				jQuery.dequeue( elem, type );
			};

		// If the fx queue is dequeued, always remove the progress sentinel
		if ( fn === "inprogress" ) {
			fn = queue.shift();
			startLength--;
		}

		if ( fn ) {

			// Add a progress sentinel to prevent the fx queue from being
			// automatically dequeued
			if ( type === "fx" ) {
				queue.unshift( "inprogress" );
			}

			// Clear up the last queue stop function
			delete hooks.stop;
			fn.call( elem, next, hooks );
		}

		if ( !startLength && hooks ) {
			hooks.empty.fire();
		}
	},

	// Not public - generate a queueHooks object, or return the current one
	_queueHooks: function( elem, type ) {
		var key = type + "queueHooks";
		return dataPriv.get( elem, key ) || dataPriv.access( elem, key, {
			empty: jQuery.Callbacks( "once memory" ).add( function() {
				dataPriv.remove( elem, [ type + "queue", key ] );
			} )
		} );
	}
} );

jQuery.fn.extend( {
	queue: function( type, data ) {
		var setter = 2;

		if ( typeof type !== "string" ) {
			data = type;
			type = "fx";
			setter--;
		}

		if ( arguments.length < setter ) {
			return jQuery.queue( this[ 0 ], type );
		}

		return data === undefined ?
			this :
			this.each( function() {
				var queue = jQuery.queue( this, type, data );

				// Ensure a hooks for this queue
				jQuery._queueHooks( this, type );

				if ( type === "fx" && queue[ 0 ] !== "inprogress" ) {
					jQuery.dequeue( this, type );
				}
			} );
	},
	dequeue: function( type ) {
		return this.each( function() {
			jQuery.dequeue( this, type );
		} );
	},
	clearQueue: function( type ) {
		return this.queue( type || "fx", [] );
	},

	// Get a promise resolved when queues of a certain type
	// are emptied (fx is the type by default)
	promise: function( type, obj ) {
		var tmp,
			count = 1,
			defer = jQuery.Deferred(),
			elements = this,
			i = this.length,
			resolve = function() {
				if ( !( --count ) ) {
					defer.resolveWith( elements, [ elements ] );
				}
			};

		if ( typeof type !== "string" ) {
			obj = type;
			type = undefined;
		}
		type = type || "fx";

		while ( i-- ) {
			tmp = dataPriv.get( elements[ i ], type + "queueHooks" );
			if ( tmp && tmp.empty ) {
				count++;
				tmp.empty.add( resolve );
			}
		}
		resolve();
		return defer.promise( obj );
	}
} );
var pnum = ( /[+-]?(?:\d*\.|)\d+(?:[eE][+-]?\d+|)/ ).source;

var rcssNum = new RegExp( "^(?:([+-])=|)(" + pnum + ")([a-z%]*)$", "i" );


var cssExpand = [ "Top", "Right", "Bottom", "Left" ];

var documentElement = document.documentElement;



	var isAttached = function( elem ) {
			return jQuery.contains( elem.ownerDocument, elem );
		},
		composed = { composed: true };

	// Support: IE 9 - 11+, Edge 12 - 18+, iOS 10.0 - 10.2 only
	// Check attachment across shadow DOM boundaries when possible (gh-3504)
	// Support: iOS 10.0-10.2 only
	// Early iOS 10 versions support `attachShadow` but not `getRootNode`,
	// leading to errors. We need to check for `getRootNode`.
	if ( documentElement.getRootNode ) {
		isAttached = function( elem ) {
			return jQuery.contains( elem.ownerDocument, elem ) ||
				elem.getRootNode( composed ) === elem.ownerDocument;
		};
	}
var isHiddenWithinTree = function( elem, el ) {

		// isHiddenWithinTree might be called from jQuery#filter function;
		// in that case, element will be second argument
		elem = el || elem;

		// Inline style trumps all
		return elem.style.display === "none" ||
			elem.style.display === "" &&

			// Otherwise, check computed style
			// Support: Firefox <=43 - 45
			// Disconnected elements can have computed display: none, so first confirm that elem is
			// in the document.
			isAttached( elem ) &&

			jQuery.css( elem, "display" ) === "none";
	};



function adjustCSS( elem, prop, valueParts, tween ) {
	var adjusted, scale,
		maxIterations = 20,
		currentValue = tween ?
			function() {
				return tween.cur();
			} :
			function() {
				return jQuery.css( elem, prop, "" );
			},
		initial = currentValue(),
		unit = valueParts && valueParts[ 3 ] || ( jQuery.cssNumber[ prop ] ? "" : "px" ),

		// Starting value computation is required for potential unit mismatches
		initialInUnit = elem.nodeType &&
			( jQuery.cssNumber[ prop ] || unit !== "px" && +initial ) &&
			rcssNum.exec( jQuery.css( elem, prop ) );

	if ( initialInUnit && initialInUnit[ 3 ] !== unit ) {

		// Support: Firefox <=54
		// Halve the iteration target value to prevent interference from CSS upper bounds (gh-2144)
		initial = initial / 2;

		// Trust units reported by jQuery.css
		unit = unit || initialInUnit[ 3 ];

		// Iteratively approximate from a nonzero starting point
		initialInUnit = +initial || 1;

		while ( maxIterations-- ) {

			// Evaluate and update our best guess (doubling guesses that zero out).
			// Finish if the scale equals or crosses 1 (making the old*new product non-positive).
			jQuery.style( elem, prop, initialInUnit + unit );
			if ( ( 1 - scale ) * ( 1 - ( scale = currentValue() / initial || 0.5 ) ) <= 0 ) {
				maxIterations = 0;
			}
			initialInUnit = initialInUnit / scale;

		}

		initialInUnit = initialInUnit * 2;
		jQuery.style( elem, prop, initialInUnit + unit );

		// Make sure we update the tween properties later on
		valueParts = valueParts || [];
	}

	if ( valueParts ) {
		initialInUnit = +initialInUnit || +initial || 0;

		// Apply relative offset (+=/-=) if specified
		adjusted = valueParts[ 1 ] ?
			initialInUnit + ( valueParts[ 1 ] + 1 ) * valueParts[ 2 ] :
			+valueParts[ 2 ];
		if ( tween ) {
			tween.unit = unit;
			tween.start = initialInUnit;
			tween.end = adjusted;
		}
	}
	return adjusted;
}


var defaultDisplayMap = {};

function getDefaultDisplay( elem ) {
	var temp,
		doc = elem.ownerDocument,
		nodeName = elem.nodeName,
		display = defaultDisplayMap[ nodeName ];

	if ( display ) {
		return display;
	}

	temp = doc.body.appendChild( doc.createElement( nodeName ) );
	display = jQuery.css( temp, "display" );

	temp.parentNode.removeChild( temp );

	if ( display === "none" ) {
		display = "block";
	}
	defaultDisplayMap[ nodeName ] = display;

	return display;
}

function showHide( elements, show ) {
	var display, elem,
		values = [],
		index = 0,
		length = elements.length;

	// Determine new display value for elements that need to change
	for ( ; index < length; index++ ) {
		elem = elements[ index ];
		if ( !elem.style ) {
			continue;
		}

		display = elem.style.display;
		if ( show ) {

			// Since we force visibility upon cascade-hidden elements, an immediate (and slow)
			// check is required in this first loop unless we have a nonempty display value (either
			// inline or about-to-be-restored)
			if ( display === "none" ) {
				values[ index ] = dataPriv.get( elem, "display" ) || null;
				if ( !values[ index ] ) {
					elem.style.display = "";
				}
			}
			if ( elem.style.display === "" && isHiddenWithinTree( elem ) ) {
				values[ index ] = getDefaultDisplay( elem );
			}
		} else {
			if ( display !== "none" ) {
				values[ index ] = "none";

				// Remember what we're overwriting
				dataPriv.set( elem, "display", display );
			}
		}
	}

	// Set the display of the elements in a second loop to avoid constant reflow
	for ( index = 0; index < length; index++ ) {
		if ( values[ index ] != null ) {
			elements[ index ].style.display = values[ index ];
		}
	}

	return elements;
}

jQuery.fn.extend( {
	show: function() {
		return showHide( this, true );
	},
	hide: function() {
		return showHide( this );
	},
	toggle: function( state ) {
		if ( typeof state === "boolean" ) {
			return state ? this.show() : this.hide();
		}

		return this.each( function() {
			if ( isHiddenWithinTree( this ) ) {
				jQuery( this ).show();
			} else {
				jQuery( this ).hide();
			}
		} );
	}
} );
var rcheckableType = ( /^(?:checkbox|radio)$/i );

var rtagName = ( /<([a-z][^\/\0>\x20\t\r\n\f]*)/i );

var rscriptType = ( /^$|^module$|\/(?:java|ecma)script/i );



( function() {
	var fragment = document.createDocumentFragment(),
		div = fragment.appendChild( document.createElement( "div" ) ),
		input = document.createElement( "input" );

	// Support: Android 4.0 - 4.3 only
	// Check state lost if the name is set (#11217)
	// Support: Windows Web Apps (WWA)
	// `name` and `type` must use .setAttribute for WWA (#14901)
	input.setAttribute( "type", "radio" );
	input.setAttribute( "checked", "checked" );
	input.setAttribute( "name", "t" );

	div.appendChild( input );

	// Support: Android <=4.1 only
	// Older WebKit doesn't clone checked state correctly in fragments
	support.checkClone = div.cloneNode( true ).cloneNode( true ).lastChild.checked;

	// Support: IE <=11 only
	// Make sure textarea (and checkbox) defaultValue is properly cloned
	div.innerHTML = "<textarea>x</textarea>";
	support.noCloneChecked = !!div.cloneNode( true ).lastChild.defaultValue;

	// Support: IE <=9 only
	// IE <=9 replaces <option> tags with their contents when inserted outside of
	// the select element.
	div.innerHTML = "<option></option>";
	support.option = !!div.lastChild;
} )();


// We have to close these tags to support XHTML (#13200)
var wrapMap = {

	// XHTML parsers do not magically insert elements in the
	// same way that tag soup parsers do. So we cannot shorten
	// this by omitting <tbody> or other required elements.
	thead: [ 1, "<table>", "</table>" ],
	col: [ 2, "<table><colgroup>", "</colgroup></table>" ],
	tr: [ 2, "<table><tbody>", "</tbody></table>" ],
	td: [ 3, "<table><tbody><tr>", "</tr></tbody></table>" ],

	_default: [ 0, "", "" ]
};

wrapMap.tbody = wrapMap.tfoot = wrapMap.colgroup = wrapMap.caption = wrapMap.thead;
wrapMap.th = wrapMap.td;

// Support: IE <=9 only
if ( !support.option ) {
	wrapMap.optgroup = wrapMap.option = [ 1, "<select multiple='multiple'>", "</select>" ];
}


function getAll( context, tag ) {

	// Support: IE <=9 - 11 only
	// Use typeof to avoid zero-argument method invocation on host objects (#15151)
	var ret;

	if ( typeof context.getElementsByTagName !== "undefined" ) {
		ret = context.getElementsByTagName( tag || "*" );

	} else if ( typeof context.querySelectorAll !== "undefined" ) {
		ret = context.querySelectorAll( tag || "*" );

	} else {
		ret = [];
	}

	if ( tag === undefined || tag && nodeName( context, tag ) ) {
		return jQuery.merge( [ context ], ret );
	}

	return ret;
}


// Mark scripts as having already been evaluated
function setGlobalEval( elems, refElements ) {
	var i = 0,
		l = elems.length;

	for ( ; i < l; i++ ) {
		dataPriv.set(
			elems[ i ],
			"globalEval",
			!refElements || dataPriv.get( refElements[ i ], "globalEval" )
		);
	}
}


var rhtml = /<|&#?\w+;/;

function buildFragment( elems, context, scripts, selection, ignored ) {
	var elem, tmp, tag, wrap, attached, j,
		fragment = context.createDocumentFragment(),
		nodes = [],
		i = 0,
		l = elems.length;

	for ( ; i < l; i++ ) {
		elem = elems[ i ];

		if ( elem || elem === 0 ) {

			// Add nodes directly
			if ( toType( elem ) === "object" ) {

				// Support: Android <=4.0 only, PhantomJS 1 only
				// push.apply(_, arraylike) throws on ancient WebKit
				jQuery.merge( nodes, elem.nodeType ? [ elem ] : elem );

			// Convert non-html into a text node
			} else if ( !rhtml.test( elem ) ) {
				nodes.push( context.createTextNode( elem ) );

			// Convert html into DOM nodes
			} else {
				tmp = tmp || fragment.appendChild( context.createElement( "div" ) );

				// Deserialize a standard representation
				tag = ( rtagName.exec( elem ) || [ "", "" ] )[ 1 ].toLowerCase();
				wrap = wrapMap[ tag ] || wrapMap._default;
				tmp.innerHTML = wrap[ 1 ] + jQuery.htmlPrefilter( elem ) + wrap[ 2 ];

				// Descend through wrappers to the right content
				j = wrap[ 0 ];
				while ( j-- ) {
					tmp = tmp.lastChild;
				}

				// Support: Android <=4.0 only, PhantomJS 1 only
				// push.apply(_, arraylike) throws on ancient WebKit
				jQuery.merge( nodes, tmp.childNodes );

				// Remember the top-level container
				tmp = fragment.firstChild;

				// Ensure the created nodes are orphaned (#12392)
				tmp.textContent = "";
			}
		}
	}

	// Remove wrapper from fragment
	fragment.textContent = "";

	i = 0;
	while ( ( elem = nodes[ i++ ] ) ) {

		// Skip elements already in the context collection (trac-4087)
		if ( selection && jQuery.inArray( elem, selection ) > -1 ) {
			if ( ignored ) {
				ignored.push( elem );
			}
			continue;
		}

		attached = isAttached( elem );

		// Append to fragment
		tmp = getAll( fragment.appendChild( elem ), "script" );

		// Preserve script evaluation history
		if ( attached ) {
			setGlobalEval( tmp );
		}

		// Capture executables
		if ( scripts ) {
			j = 0;
			while ( ( elem = tmp[ j++ ] ) ) {
				if ( rscriptType.test( elem.type || "" ) ) {
					scripts.push( elem );
				}
			}
		}
	}

	return fragment;
}


var rtypenamespace = /^([^.]*)(?:\.(.+)|)/;

function returnTrue() {
	return true;
}

function returnFalse() {
	return false;
}

// Support: IE <=9 - 11+
// focus() and blur() are asynchronous, except when they are no-op.
// So expect focus to be synchronous when the element is already active,
// and blur to be synchronous when the element is not already active.
// (focus and blur are always synchronous in other supported browsers,
// this just defines when we can count on it).
function expectSync( elem, type ) {
	return ( elem === safeActiveElement() ) === ( type === "focus" );
}

// Support: IE <=9 only
// Accessing document.activeElement can throw unexpectedly
// https://bugs.jquery.com/ticket/13393
function safeActiveElement() {
	try {
		return document.activeElement;
	} catch ( err ) { }
}

function on( elem, types, selector, data, fn, one ) {
	var origFn, type;

	// Types can be a map of types/handlers
	if ( typeof types === "object" ) {

		// ( types-Object, selector, data )
		if ( typeof selector !== "string" ) {

			// ( types-Object, data )
			data = data || selector;
			selector = undefined;
		}
		for ( type in types ) {
			on( elem, type, selector, data, types[ type ], one );
		}
		return elem;
	}

	if ( data == null && fn == null ) {

		// ( types, fn )
		fn = selector;
		data = selector = undefined;
	} else if ( fn == null ) {
		if ( typeof selector === "string" ) {

			// ( types, selector, fn )
			fn = data;
			data = undefined;
		} else {

			// ( types, data, fn )
			fn = data;
			data = selector;
			selector = undefined;
		}
	}
	if ( fn === false ) {
		fn = returnFalse;
	} else if ( !fn ) {
		return elem;
	}

	if ( one === 1 ) {
		origFn = fn;
		fn = function( event ) {

			// Can use an empty set, since event contains the info
			jQuery().off( event );
			return origFn.apply( this, arguments );
		};

		// Use same guid so caller can remove using origFn
		fn.guid = origFn.guid || ( origFn.guid = jQuery.guid++ );
	}
	return elem.each( function() {
		jQuery.event.add( this, types, fn, data, selector );
	} );
}

/*
 * Helper functions for managing events -- not part of the public interface.
 * Props to Dean Edwards' addEvent library for many of the ideas.
 */
jQuery.event = {

	global: {},

	add: function( elem, types, handler, data, selector ) {

		var handleObjIn, eventHandle, tmp,
			events, t, handleObj,
			special, handlers, type, namespaces, origType,
			elemData = dataPriv.get( elem );

		// Only attach events to objects that accept data
		if ( !acceptData( elem ) ) {
			return;
		}

		// Caller can pass in an object of custom data in lieu of the handler
		if ( handler.handler ) {
			handleObjIn = handler;
			handler = handleObjIn.handler;
			selector = handleObjIn.selector;
		}

		// Ensure that invalid selectors throw exceptions at attach time
		// Evaluate against documentElement in case elem is a non-element node (e.g., document)
		if ( selector ) {
			jQuery.find.matchesSelector( documentElement, selector );
		}

		// Make sure that the handler has a unique ID, used to find/remove it later
		if ( !handler.guid ) {
			handler.guid = jQuery.guid++;
		}

		// Init the element's event structure and main handler, if this is the first
		if ( !( events = elemData.events ) ) {
			events = elemData.events = Object.create( null );
		}
		if ( !( eventHandle = elemData.handle ) ) {
			eventHandle = elemData.handle = function( e ) {

				// Discard the second event of a jQuery.event.trigger() and
				// when an event is called after a page has unloaded
				return typeof jQuery !== "undefined" && jQuery.event.triggered !== e.type ?
					jQuery.event.dispatch.apply( elem, arguments ) : undefined;
			};
		}

		// Handle multiple events separated by a space
		types = ( types || "" ).match( rnothtmlwhite ) || [ "" ];
		t = types.length;
		while ( t-- ) {
			tmp = rtypenamespace.exec( types[ t ] ) || [];
			type = origType = tmp[ 1 ];
			namespaces = ( tmp[ 2 ] || "" ).split( "." ).sort();

			// There *must* be a type, no attaching namespace-only handlers
			if ( !type ) {
				continue;
			}

			// If event changes its type, use the special event handlers for the changed type
			special = jQuery.event.special[ type ] || {};

			// If selector defined, determine special event api type, otherwise given type
			type = ( selector ? special.delegateType : special.bindType ) || type;

			// Update special based on newly reset type
			special = jQuery.event.special[ type ] || {};

			// handleObj is passed to all event handlers
			handleObj = jQuery.extend( {
				type: type,
				origType: origType,
				data: data,
				handler: handler,
				guid: handler.guid,
				selector: selector,
				needsContext: selector && jQuery.expr.match.needsContext.test( selector ),
				namespace: namespaces.join( "." )
			}, handleObjIn );

			// Init the event handler queue if we're the first
			if ( !( handlers = events[ type ] ) ) {
				handlers = events[ type ] = [];
				handlers.delegateCount = 0;

				// Only use addEventListener if the special events handler returns false
				if ( !special.setup ||
					special.setup.call( elem, data, namespaces, eventHandle ) === false ) {

					if ( elem.addEventListener ) {
						elem.addEventListener( type, eventHandle );
					}
				}
			}

			if ( special.add ) {
				special.add.call( elem, handleObj );

				if ( !handleObj.handler.guid ) {
					handleObj.handler.guid = handler.guid;
				}
			}

			// Add to the element's handler list, delegates in front
			if ( selector ) {
				handlers.splice( handlers.delegateCount++, 0, handleObj );
			} else {
				handlers.push( handleObj );
			}

			// Keep track of which events have ever been used, for event optimization
			jQuery.event.global[ type ] = true;
		}

	},

	// Detach an event or set of events from an element
	remove: function( elem, types, handler, selector, mappedTypes ) {

		var j, origCount, tmp,
			events, t, handleObj,
			special, handlers, type, namespaces, origType,
			elemData = dataPriv.hasData( elem ) && dataPriv.get( elem );

		if ( !elemData || !( events = elemData.events ) ) {
			return;
		}

		// Once for each type.namespace in types; type may be omitted
		types = ( types || "" ).match( rnothtmlwhite ) || [ "" ];
		t = types.length;
		while ( t-- ) {
			tmp = rtypenamespace.exec( types[ t ] ) || [];
			type = origType = tmp[ 1 ];
			namespaces = ( tmp[ 2 ] || "" ).split( "." ).sort();

			// Unbind all events (on this namespace, if provided) for the element
			if ( !type ) {
				for ( type in events ) {
					jQuery.event.remove( elem, type + types[ t ], handler, selector, true );
				}
				continue;
			}

			special = jQuery.event.special[ type ] || {};
			type = ( selector ? special.delegateType : special.bindType ) || type;
			handlers = events[ type ] || [];
			tmp = tmp[ 2 ] &&
				new RegExp( "(^|\\.)" + namespaces.join( "\\.(?:.*\\.|)" ) + "(\\.|$)" );

			// Remove matching events
			origCount = j = handlers.length;
			while ( j-- ) {
				handleObj = handlers[ j ];

				if ( ( mappedTypes || origType === handleObj.origType ) &&
					( !handler || handler.guid === handleObj.guid ) &&
					( !tmp || tmp.test( handleObj.namespace ) ) &&
					( !selector || selector === handleObj.selector ||
						selector === "**" && handleObj.selector ) ) {
					handlers.splice( j, 1 );

					if ( handleObj.selector ) {
						handlers.delegateCount--;
					}
					if ( special.remove ) {
						special.remove.call( elem, handleObj );
					}
				}
			}

			// Remove generic event handler if we removed something and no more handlers exist
			// (avoids potential for endless recursion during removal of special event handlers)
			if ( origCount && !handlers.length ) {
				if ( !special.teardown ||
					special.teardown.call( elem, namespaces, elemData.handle ) === false ) {

					jQuery.removeEvent( elem, type, elemData.handle );
				}

				delete events[ type ];
			}
		}

		// Remove data and the expando if it's no longer used
		if ( jQuery.isEmptyObject( events ) ) {
			dataPriv.remove( elem, "handle events" );
		}
	},

	dispatch: function( nativeEvent ) {

		var i, j, ret, matched, handleObj, handlerQueue,
			args = new Array( arguments.length ),

			// Make a writable jQuery.Event from the native event object
			event = jQuery.event.fix( nativeEvent ),

			handlers = (
				dataPriv.get( this, "events" ) || Object.create( null )
			)[ event.type ] || [],
			special = jQuery.event.special[ event.type ] || {};

		// Use the fix-ed jQuery.Event rather than the (read-only) native event
		args[ 0 ] = event;

		for ( i = 1; i < arguments.length; i++ ) {
			args[ i ] = arguments[ i ];
		}

		event.delegateTarget = this;

		// Call the preDispatch hook for the mapped type, and let it bail if desired
		if ( special.preDispatch && special.preDispatch.call( this, event ) === false ) {
			return;
		}

		// Determine handlers
		handlerQueue = jQuery.event.handlers.call( this, event, handlers );

		// Run delegates first; they may want to stop propagation beneath us
		i = 0;
		while ( ( matched = handlerQueue[ i++ ] ) && !event.isPropagationStopped() ) {
			event.currentTarget = matched.elem;

			j = 0;
			while ( ( handleObj = matched.handlers[ j++ ] ) &&
				!event.isImmediatePropagationStopped() ) {

				// If the event is namespaced, then each handler is only invoked if it is
				// specially universal or its namespaces are a superset of the event's.
				if ( !event.rnamespace || handleObj.namespace === false ||
					event.rnamespace.test( handleObj.namespace ) ) {

					event.handleObj = handleObj;
					event.data = handleObj.data;

					ret = ( ( jQuery.event.special[ handleObj.origType ] || {} ).handle ||
						handleObj.handler ).apply( matched.elem, args );

					if ( ret !== undefined ) {
						if ( ( event.result = ret ) === false ) {
							event.preventDefault();
							event.stopPropagation();
						}
					}
				}
			}
		}

		// Call the postDispatch hook for the mapped type
		if ( special.postDispatch ) {
			special.postDispatch.call( this, event );
		}

		return event.result;
	},

	handlers: function( event, handlers ) {
		var i, handleObj, sel, matchedHandlers, matchedSelectors,
			handlerQueue = [],
			delegateCount = handlers.delegateCount,
			cur = event.target;

		// Find delegate handlers
		if ( delegateCount &&

			// Support: IE <=9
			// Black-hole SVG <use> instance trees (trac-13180)
			cur.nodeType &&

			// Support: Firefox <=42
			// Suppress spec-violating clicks indicating a non-primary pointer button (trac-3861)
			// https://www.w3.org/TR/DOM-Level-3-Events/#event-type-click
			// Support: IE 11 only
			// ...but not arrow key "clicks" of radio inputs, which can have `button` -1 (gh-2343)
			!( event.type === "click" && event.button >= 1 ) ) {

			for ( ; cur !== this; cur = cur.parentNode || this ) {

				// Don't check non-elements (#13208)
				// Don't process clicks on disabled elements (#6911, #8165, #11382, #11764)
				if ( cur.nodeType === 1 && !( event.type === "click" && cur.disabled === true ) ) {
					matchedHandlers = [];
					matchedSelectors = {};
					for ( i = 0; i < delegateCount; i++ ) {
						handleObj = handlers[ i ];

						// Don't conflict with Object.prototype properties (#13203)
						sel = handleObj.selector + " ";

						if ( matchedSelectors[ sel ] === undefined ) {
							matchedSelectors[ sel ] = handleObj.needsContext ?
								jQuery( sel, this ).index( cur ) > -1 :
								jQuery.find( sel, this, null, [ cur ] ).length;
						}
						if ( matchedSelectors[ sel ] ) {
							matchedHandlers.push( handleObj );
						}
					}
					if ( matchedHandlers.length ) {
						handlerQueue.push( { elem: cur, handlers: matchedHandlers } );
					}
				}
			}
		}

		// Add the remaining (directly-bound) handlers
		cur = this;
		if ( delegateCount < handlers.length ) {
			handlerQueue.push( { elem: cur, handlers: handlers.slice( delegateCount ) } );
		}

		return handlerQueue;
	},

	addProp: function( name, hook ) {
		Object.defineProperty( jQuery.Event.prototype, name, {
			enumerable: true,
			configurable: true,

			get: isFunction( hook ) ?
				function() {
					if ( this.originalEvent ) {
						return hook( this.originalEvent );
					}
				} :
				function() {
					if ( this.originalEvent ) {
						return this.originalEvent[ name ];
					}
				},

			set: function( value ) {
				Object.defineProperty( this, name, {
					enumerable: true,
					configurable: true,
					writable: true,
					value: value
				} );
			}
		} );
	},

	fix: function( originalEvent ) {
		return originalEvent[ jQuery.expando ] ?
			originalEvent :
			new jQuery.Event( originalEvent );
	},

	special: {
		load: {

			// Prevent triggered image.load events from bubbling to window.load
			noBubble: true
		},
		click: {

			// Utilize native event to ensure correct state for checkable inputs
			setup: function( data ) {

				// For mutual compressibility with _default, replace `this` access with a local var.
				// `|| data` is dead code meant only to preserve the variable through minification.
				var el = this || data;

				// Claim the first handler
				if ( rcheckableType.test( el.type ) &&
					el.click && nodeName( el, "input" ) ) {

					// dataPriv.set( el, "click", ... )
					leverageNative( el, "click", returnTrue );
				}

				// Return false to allow normal processing in the caller
				return false;
			},
			trigger: function( data ) {

				// For mutual compressibility with _default, replace `this` access with a local var.
				// `|| data` is dead code meant only to preserve the variable through minification.
				var el = this || data;

				// Force setup before triggering a click
				if ( rcheckableType.test( el.type ) &&
					el.click && nodeName( el, "input" ) ) {

					leverageNative( el, "click" );
				}

				// Return non-false to allow normal event-path propagation
				return true;
			},

			// For cross-browser consistency, suppress native .click() on links
			// Also prevent it if we're currently inside a leveraged native-event stack
			_default: function( event ) {
				var target = event.target;
				return rcheckableType.test( target.type ) &&
					target.click && nodeName( target, "input" ) &&
					dataPriv.get( target, "click" ) ||
					nodeName( target, "a" );
			}
		},

		beforeunload: {
			postDispatch: function( event ) {

				// Support: Firefox 20+
				// Firefox doesn't alert if the returnValue field is not set.
				if ( event.result !== undefined && event.originalEvent ) {
					event.originalEvent.returnValue = event.result;
				}
			}
		}
	}
};

// Ensure the presence of an event listener that handles manually-triggered
// synthetic events by interrupting progress until reinvoked in response to
// *native* events that it fires directly, ensuring that state changes have
// already occurred before other listeners are invoked.
function leverageNative( el, type, expectSync ) {

	// Missing expectSync indicates a trigger call, which must force setup through jQuery.event.add
	if ( !expectSync ) {
		if ( dataPriv.get( el, type ) === undefined ) {
			jQuery.event.add( el, type, returnTrue );
		}
		return;
	}

	// Register the controller as a special universal handler for all event namespaces
	dataPriv.set( el, type, false );
	jQuery.event.add( el, type, {
		namespace: false,
		handler: function( event ) {
			var notAsync, result,
				saved = dataPriv.get( this, type );

			if ( ( event.isTrigger & 1 ) && this[ type ] ) {

				// Interrupt processing of the outer synthetic .trigger()ed event
				// Saved data should be false in such cases, but might be a leftover capture object
				// from an async native handler (gh-4350)
				if ( !saved.length ) {

					// Store arguments for use when handling the inner native event
					// There will always be at least one argument (an event object), so this array
					// will not be confused with a leftover capture object.
					saved = slice.call( arguments );
					dataPriv.set( this, type, saved );

					// Trigger the native event and capture its result
					// Support: IE <=9 - 11+
					// focus() and blur() are asynchronous
					notAsync = expectSync( this, type );
					this[ type ]();
					result = dataPriv.get( this, type );
					if ( saved !== result || notAsync ) {
						dataPriv.set( this, type, false );
					} else {
						result = {};
					}
					if ( saved !== result ) {

						// Cancel the outer synthetic event
						event.stopImmediatePropagation();
						event.preventDefault();

						// Support: Chrome 86+
						// In Chrome, if an element having a focusout handler is blurred by
						// clicking outside of it, it invokes the handler synchronously. If
						// that handler calls `.remove()` on the element, the data is cleared,
						// leaving `result` undefined. We need to guard against this.
						return result && result.value;
					}

				// If this is an inner synthetic event for an event with a bubbling surrogate
				// (focus or blur), assume that the surrogate already propagated from triggering the
				// native event and prevent that from happening again here.
				// This technically gets the ordering wrong w.r.t. to `.trigger()` (in which the
				// bubbling surrogate propagates *after* the non-bubbling base), but that seems
				// less bad than duplication.
				} else if ( ( jQuery.event.special[ type ] || {} ).delegateType ) {
					event.stopPropagation();
				}

			// If this is a native event triggered above, everything is now in order
			// Fire an inner synthetic event with the original arguments
			} else if ( saved.length ) {

				// ...and capture the result
				dataPriv.set( this, type, {
					value: jQuery.event.trigger(

						// Support: IE <=9 - 11+
						// Extend with the prototype to reset the above stopImmediatePropagation()
						jQuery.extend( saved[ 0 ], jQuery.Event.prototype ),
						saved.slice( 1 ),
						this
					)
				} );

				// Abort handling of the native event
				event.stopImmediatePropagation();
			}
		}
	} );
}

jQuery.removeEvent = function( elem, type, handle ) {

	// This "if" is needed for plain objects
	if ( elem.removeEventListener ) {
		elem.removeEventListener( type, handle );
	}
};

jQuery.Event = function( src, props ) {

	// Allow instantiation without the 'new' keyword
	if ( !( this instanceof jQuery.Event ) ) {
		return new jQuery.Event( src, props );
	}

	// Event object
	if ( src && src.type ) {
		this.originalEvent = src;
		this.type = src.type;

		// Events bubbling up the document may have been marked as prevented
		// by a handler lower down the tree; reflect the correct value.
		this.isDefaultPrevented = src.defaultPrevented ||
				src.defaultPrevented === undefined &&

				// Support: Android <=2.3 only
				src.returnValue === false ?
			returnTrue :
			returnFalse;

		// Create target properties
		// Support: Safari <=6 - 7 only
		// Target should not be a text node (#504, #13143)
		this.target = ( src.target && src.target.nodeType === 3 ) ?
			src.target.parentNode :
			src.target;

		this.currentTarget = src.currentTarget;
		this.relatedTarget = src.relatedTarget;

	// Event type
	} else {
		this.type = src;
	}

	// Put explicitly provided properties onto the event object
	if ( props ) {
		jQuery.extend( this, props );
	}

	// Create a timestamp if incoming event doesn't have one
	this.timeStamp = src && src.timeStamp || Date.now();

	// Mark it as fixed
	this[ jQuery.expando ] = true;
};

// jQuery.Event is based on DOM3 Events as specified by the ECMAScript Language Binding
// https://www.w3.org/TR/2003/WD-DOM-Level-3-Events-20030331/ecma-script-binding.html
jQuery.Event.prototype = {
	constructor: jQuery.Event,
	isDefaultPrevented: returnFalse,
	isPropagationStopped: returnFalse,
	isImmediatePropagationStopped: returnFalse,
	isSimulated: false,

	preventDefault: function() {
		var e = this.originalEvent;

		this.isDefaultPrevented = returnTrue;

		if ( e && !this.isSimulated ) {
			e.preventDefault();
		}
	},
	stopPropagation: function() {
		var e = this.originalEvent;

		this.isPropagationStopped = returnTrue;

		if ( e && !this.isSimulated ) {
			e.stopPropagation();
		}
	},
	stopImmediatePropagation: function() {
		var e = this.originalEvent;

		this.isImmediatePropagationStopped = returnTrue;

		if ( e && !this.isSimulated ) {
			e.stopImmediatePropagation();
		}

		this.stopPropagation();
	}
};

// Includes all common event props including KeyEvent and MouseEvent specific props
jQuery.each( {
	altKey: true,
	bubbles: true,
	cancelable: true,
	changedTouches: true,
	ctrlKey: true,
	detail: true,
	eventPhase: true,
	metaKey: true,
	pageX: true,
	pageY: true,
	shiftKey: true,
	view: true,
	"char": true,
	code: true,
	charCode: true,
	key: true,
	keyCode: true,
	button: true,
	buttons: true,
	clientX: true,
	clientY: true,
	offsetX: true,
	offsetY: true,
	pointerId: true,
	pointerType: true,
	screenX: true,
	screenY: true,
	targetTouches: true,
	toElement: true,
	touches: true,
	which: true
}, jQuery.event.addProp );

jQuery.each( { focus: "focusin", blur: "focusout" }, function( type, delegateType ) {
	jQuery.event.special[ type ] = {

		// Utilize native event if possible so blur/focus sequence is correct
		setup: function() {

			// Claim the first handler
			// dataPriv.set( this, "focus", ... )
			// dataPriv.set( this, "blur", ... )
			leverageNative( this, type, expectSync );

			// Return false to allow normal processing in the caller
			return false;
		},
		trigger: function() {

			// Force setup before trigger
			leverageNative( this, type );

			// Return non-false to allow normal event-path propagation
			return true;
		},

		// Suppress native focus or blur as it's already being fired
		// in leverageNative.
		_default: function() {
			return true;
		},

		delegateType: delegateType
	};
} );

// Create mouseenter/leave events using mouseover/out and event-time checks
// so that event delegation works in jQuery.
// Do the same for pointerenter/pointerleave and pointerover/pointerout
//
// Support: Safari 7 only
// Safari sends mouseenter too often; see:
// https://bugs.chromium.org/p/chromium/issues/detail?id=470258
// for the description of the bug (it existed in older Chrome versions as well).
jQuery.each( {
	mouseenter: "mouseover",
	mouseleave: "mouseout",
	pointerenter: "pointerover",
	pointerleave: "pointerout"
}, function( orig, fix ) {
	jQuery.event.special[ orig ] = {
		delegateType: fix,
		bindType: fix,

		handle: function( event ) {
			var ret,
				target = this,
				related = event.relatedTarget,
				handleObj = event.handleObj;

			// For mouseenter/leave call the handler if related is outside the target.
			// NB: No relatedTarget if the mouse left/entered the browser window
			if ( !related || ( related !== target && !jQuery.contains( target, related ) ) ) {
				event.type = handleObj.origType;
				ret = handleObj.handler.apply( this, arguments );
				event.type = fix;
			}
			return ret;
		}
	};
} );

jQuery.fn.extend( {

	on: function( types, selector, data, fn ) {
		return on( this, types, selector, data, fn );
	},
	one: function( types, selector, data, fn ) {
		return on( this, types, selector, data, fn, 1 );
	},
	off: function( types, selector, fn ) {
		var handleObj, type;
		if ( types && types.preventDefault && types.handleObj ) {

			// ( event )  dispatched jQuery.Event
			handleObj = types.handleObj;
			jQuery( types.delegateTarget ).off(
				handleObj.namespace ?
					handleObj.origType + "." + handleObj.namespace :
					handleObj.origType,
				handleObj.selector,
				handleObj.handler
			);
			return this;
		}
		if ( typeof types === "object" ) {

			// ( types-object [, selector] )
			for ( type in types ) {
				this.off( type, selector, types[ type ] );
			}
			return this;
		}
		if ( selector === false || typeof selector === "function" ) {

			// ( types [, fn] )
			fn = selector;
			selector = undefined;
		}
		if ( fn === false ) {
			fn = returnFalse;
		}
		return this.each( function() {
			jQuery.event.remove( this, types, fn, selector );
		} );
	}
} );


var

	// Support: IE <=10 - 11, Edge 12 - 13 only
	// In IE/Edge using regex groups here causes severe slowdowns.
	// See https://connect.microsoft.com/IE/feedback/details/1736512/
	rnoInnerhtml = /<script|<style|<link/i,

	// checked="checked" or checked
	rchecked = /checked\s*(?:[^=]|=\s*.checked.)/i,
	rcleanScript = /^\s*<!(?:\[CDATA\[|--)|(?:\]\]|--)>\s*$/g;

// Prefer a tbody over its parent table for containing new rows
function manipulationTarget( elem, content ) {
	if ( nodeName( elem, "table" ) &&
		nodeName( content.nodeType !== 11 ? content : content.firstChild, "tr" ) ) {

		return jQuery( elem ).children( "tbody" )[ 0 ] || elem;
	}

	return elem;
}

// Replace/restore the type attribute of script elements for safe DOM manipulation
function disableScript( elem ) {
	elem.type = ( elem.getAttribute( "type" ) !== null ) + "/" + elem.type;
	return elem;
}
function restoreScript( elem ) {
	if ( ( elem.type || "" ).slice( 0, 5 ) === "true/" ) {
		elem.type = elem.type.slice( 5 );
	} else {
		elem.removeAttribute( "type" );
	}

	return elem;
}

function cloneCopyEvent( src, dest ) {
	var i, l, type, pdataOld, udataOld, udataCur, events;

	if ( dest.nodeType !== 1 ) {
		return;
	}

	// 1. Copy private data: events, handlers, etc.
	if ( dataPriv.hasData( src ) ) {
		pdataOld = dataPriv.get( src );
		events = pdataOld.events;

		if ( events ) {
			dataPriv.remove( dest, "handle events" );

			for ( type in events ) {
				for ( i = 0, l = events[ type ].length; i < l; i++ ) {
					jQuery.event.add( dest, type, events[ type ][ i ] );
				}
			}
		}
	}

	// 2. Copy user data
	if ( dataUser.hasData( src ) ) {
		udataOld = dataUser.access( src );
		udataCur = jQuery.extend( {}, udataOld );

		dataUser.set( dest, udataCur );
	}
}

// Fix IE bugs, see support tests
function fixInput( src, dest ) {
	var nodeName = dest.nodeName.toLowerCase();

	// Fails to persist the checked state of a cloned checkbox or radio button.
	if ( nodeName === "input" && rcheckableType.test( src.type ) ) {
		dest.checked = src.checked;

	// Fails to return the selected option to the default selected state when cloning options
	} else if ( nodeName === "input" || nodeName === "textarea" ) {
		dest.defaultValue = src.defaultValue;
	}
}

function domManip( collection, args, callback, ignored ) {

	// Flatten any nested arrays
	args = flat( args );

	var fragment, first, scripts, hasScripts, node, doc,
		i = 0,
		l = collection.length,
		iNoClone = l - 1,
		value = args[ 0 ],
		valueIsFunction = isFunction( value );

	// We can't cloneNode fragments that contain checked, in WebKit
	if ( valueIsFunction ||
			( l > 1 && typeof value === "string" &&
				!support.checkClone && rchecked.test( value ) ) ) {
		return collection.each( function( index ) {
			var self = collection.eq( index );
			if ( valueIsFunction ) {
				args[ 0 ] = value.call( this, index, self.html() );
			}
			domManip( self, args, callback, ignored );
		} );
	}

	if ( l ) {
		fragment = buildFragment( args, collection[ 0 ].ownerDocument, false, collection, ignored );
		first = fragment.firstChild;

		if ( fragment.childNodes.length === 1 ) {
			fragment = first;
		}

		// Require either new content or an interest in ignored elements to invoke the callback
		if ( first || ignored ) {
			scripts = jQuery.map( getAll( fragment, "script" ), disableScript );
			hasScripts = scripts.length;

			// Use the original fragment for the last item
			// instead of the first because it can end up
			// being emptied incorrectly in certain situations (#8070).
			for ( ; i < l; i++ ) {
				node = fragment;

				if ( i !== iNoClone ) {
					node = jQuery.clone( node, true, true );

					// Keep references to cloned scripts for later restoration
					if ( hasScripts ) {

						// Support: Android <=4.0 only, PhantomJS 1 only
						// push.apply(_, arraylike) throws on ancient WebKit
						jQuery.merge( scripts, getAll( node, "script" ) );
					}
				}

				callback.call( collection[ i ], node, i );
			}

			if ( hasScripts ) {
				doc = scripts[ scripts.length - 1 ].ownerDocument;

				// Reenable scripts
				jQuery.map( scripts, restoreScript );

				// Evaluate executable scripts on first document insertion
				for ( i = 0; i < hasScripts; i++ ) {
					node = scripts[ i ];
					if ( rscriptType.test( node.type || "" ) &&
						!dataPriv.access( node, "globalEval" ) &&
						jQuery.contains( doc, node ) ) {

						if ( node.src && ( node.type || "" ).toLowerCase()  !== "module" ) {

							// Optional AJAX dependency, but won't run scripts if not present
							if ( jQuery._evalUrl && !node.noModule ) {
								jQuery._evalUrl( node.src, {
									nonce: node.nonce || node.getAttribute( "nonce" )
								}, doc );
							}
						} else {
							DOMEval( node.textContent.replace( rcleanScript, "" ), node, doc );
						}
					}
				}
			}
		}
	}

	return collection;
}

function remove( elem, selector, keepData ) {
	var node,
		nodes = selector ? jQuery.filter( selector, elem ) : elem,
		i = 0;

	for ( ; ( node = nodes[ i ] ) != null; i++ ) {
		if ( !keepData && node.nodeType === 1 ) {
			jQuery.cleanData( getAll( node ) );
		}

		if ( node.parentNode ) {
			if ( keepData && isAttached( node ) ) {
				setGlobalEval( getAll( node, "script" ) );
			}
			node.parentNode.removeChild( node );
		}
	}

	return elem;
}

jQuery.extend( {
	htmlPrefilter: function( html ) {
		return html;
	},

	clone: function( elem, dataAndEvents, deepDataAndEvents ) {
		var i, l, srcElements, destElements,
			clone = elem.cloneNode( true ),
			inPage = isAttached( elem );

		// Fix IE cloning issues
		if ( !support.noCloneChecked && ( elem.nodeType === 1 || elem.nodeType === 11 ) &&
				!jQuery.isXMLDoc( elem ) ) {

			// We eschew Sizzle here for performance reasons: https://jsperf.com/getall-vs-sizzle/2
			destElements = getAll( clone );
			srcElements = getAll( elem );

			for ( i = 0, l = srcElements.length; i < l; i++ ) {
				fixInput( srcElements[ i ], destElements[ i ] );
			}
		}

		// Copy the events from the original to the clone
		if ( dataAndEvents ) {
			if ( deepDataAndEvents ) {
				srcElements = srcElements || getAll( elem );
				destElements = destElements || getAll( clone );

				for ( i = 0, l = srcElements.length; i < l; i++ ) {
					cloneCopyEvent( srcElements[ i ], destElements[ i ] );
				}
			} else {
				cloneCopyEvent( elem, clone );
			}
		}

		// Preserve script evaluation history
		destElements = getAll( clone, "script" );
		if ( destElements.length > 0 ) {
			setGlobalEval( destElements, !inPage && getAll( elem, "script" ) );
		}

		// Return the cloned set
		return clone;
	},

	cleanData: function( elems ) {
		var data, elem, type,
			special = jQuery.event.special,
			i = 0;

		for ( ; ( elem = elems[ i ] ) !== undefined; i++ ) {
			if ( acceptData( elem ) ) {
				if ( ( data = elem[ dataPriv.expando ] ) ) {
					if ( data.events ) {
						for ( type in data.events ) {
							if ( special[ type ] ) {
								jQuery.event.remove( elem, type );

							// This is a shortcut to avoid jQuery.event.remove's overhead
							} else {
								jQuery.removeEvent( elem, type, data.handle );
							}
						}
					}

					// Support: Chrome <=35 - 45+
					// Assign undefined instead of using delete, see Data#remove
					elem[ dataPriv.expando ] = undefined;
				}
				if ( elem[ dataUser.expando ] ) {

					// Support: Chrome <=35 - 45+
					// Assign undefined instead of using delete, see Data#remove
					elem[ dataUser.expando ] = undefined;
				}
			}
		}
	}
} );

jQuery.fn.extend( {
	detach: function( selector ) {
		return remove( this, selector, true );
	},

	remove: function( selector ) {
		return remove( this, selector );
	},

	text: function( value ) {
		return access( this, function( value ) {
			return value === undefined ?
				jQuery.text( this ) :
				this.empty().each( function() {
					if ( this.nodeType === 1 || this.nodeType === 11 || this.nodeType === 9 ) {
						this.textContent = value;
					}
				} );
		}, null, value, arguments.length );
	},

	append: function() {
		return domManip( this, arguments, function( elem ) {
			if ( this.nodeType === 1 || this.nodeType === 11 || this.nodeType === 9 ) {
				var target = manipulationTarget( this, elem );
				target.appendChild( elem );
			}
		} );
	},

	prepend: function() {
		return domManip( this, arguments, function( elem ) {
			if ( this.nodeType === 1 || this.nodeType === 11 || this.nodeType === 9 ) {
				var target = manipulationTarget( this, elem );
				target.insertBefore( elem, target.firstChild );
			}
		} );
	},

	before: function() {
		return domManip( this, arguments, function( elem ) {
			if ( this.parentNode ) {
				this.parentNode.insertBefore( elem, this );
			}
		} );
	},

	after: function() {
		return domManip( this, arguments, function( elem ) {
			if ( this.parentNode ) {
				this.parentNode.insertBefore( elem, this.nextSibling );
			}
		} );
	},

	empty: function() {
		var elem,
			i = 0;

		for ( ; ( elem = this[ i ] ) != null; i++ ) {
			if ( elem.nodeType === 1 ) {

				// Prevent memory leaks
				jQuery.cleanData( getAll( elem, false ) );

				// Remove any remaining nodes
				elem.textContent = "";
			}
		}

		return this;
	},

	clone: function( dataAndEvents, deepDataAndEvents ) {
		dataAndEvents = dataAndEvents == null ? false : dataAndEvents;
		deepDataAndEvents = deepDataAndEvents == null ? dataAndEvents : deepDataAndEvents;

		return this.map( function() {
			return jQuery.clone( this, dataAndEvents, deepDataAndEvents );
		} );
	},

	html: function( value ) {
		return access( this, function( value ) {
			var elem = this[ 0 ] || {},
				i = 0,
				l = this.length;

			if ( value === undefined && elem.nodeType === 1 ) {
				return elem.innerHTML;
			}

			// See if we can take a shortcut and just use innerHTML
			if ( typeof value === "string" && !rnoInnerhtml.test( value ) &&
				!wrapMap[ ( rtagName.exec( value ) || [ "", "" ] )[ 1 ].toLowerCase() ] ) {

				value = jQuery.htmlPrefilter( value );

				try {
					for ( ; i < l; i++ ) {
						elem = this[ i ] || {};

						// Remove element nodes and prevent memory leaks
						if ( elem.nodeType === 1 ) {
							jQuery.cleanData( getAll( elem, false ) );
							elem.innerHTML = value;
						}
					}

					elem = 0;

				// If using innerHTML throws an exception, use the fallback method
				} catch ( e ) {}
			}

			if ( elem ) {
				this.empty().append( value );
			}
		}, null, value, arguments.length );
	},

	replaceWith: function() {
		var ignored = [];

		// Make the changes, replacing each non-ignored context element with the new content
		return domManip( this, arguments, function( elem ) {
			var parent = this.parentNode;

			if ( jQuery.inArray( this, ignored ) < 0 ) {
				jQuery.cleanData( getAll( this ) );
				if ( parent ) {
					parent.replaceChild( elem, this );
				}
			}

		// Force callback invocation
		}, ignored );
	}
} );

jQuery.each( {
	appendTo: "append",
	prependTo: "prepend",
	insertBefore: "before",
	insertAfter: "after",
	replaceAll: "replaceWith"
}, function( name, original ) {
	jQuery.fn[ name ] = function( selector ) {
		var elems,
			ret = [],
			insert = jQuery( selector ),
			last = insert.length - 1,
			i = 0;

		for ( ; i <= last; i++ ) {
			elems = i === last ? this : this.clone( true );
			jQuery( insert[ i ] )[ original ]( elems );

			// Support: Android <=4.0 only, PhantomJS 1 only
			// .get() because push.apply(_, arraylike) throws on ancient WebKit
			push.apply( ret, elems.get() );
		}

		return this.pushStack( ret );
	};
} );
var rnumnonpx = new RegExp( "^(" + pnum + ")(?!px)[a-z%]+$", "i" );

var getStyles = function( elem ) {

		// Support: IE <=11 only, Firefox <=30 (#15098, #14150)
		// IE throws on elements created in popups
		// FF meanwhile throws on frame elements through "defaultView.getComputedStyle"
		var view = elem.ownerDocument.defaultView;

		if ( !view || !view.opener ) {
			view = window;
		}

		return view.getComputedStyle( elem );
	};

var swap = function( elem, options, callback ) {
	var ret, name,
		old = {};

	// Remember the old values, and insert the new ones
	for ( name in options ) {
		old[ name ] = elem.style[ name ];
		elem.style[ name ] = options[ name ];
	}

	ret = callback.call( elem );

	// Revert the old values
	for ( name in options ) {
		elem.style[ name ] = old[ name ];
	}

	return ret;
};


var rboxStyle = new RegExp( cssExpand.join( "|" ), "i" );



( function() {

	// Executing both pixelPosition & boxSizingReliable tests require only one layout
	// so they're executed at the same time to save the second computation.
	function computeStyleTests() {

		// This is a singleton, we need to execute it only once
		if ( !div ) {
			return;
		}

		container.style.cssText = "position:absolute;left:-11111px;width:60px;" +
			"margin-top:1px;padding:0;border:0";
		div.style.cssText =
			"position:relative;display:block;box-sizing:border-box;overflow:scroll;" +
			"margin:auto;border:1px;padding:1px;" +
			"width:60%;top:1%";
		documentElement.appendChild( container ).appendChild( div );

		var divStyle = window.getComputedStyle( div );
		pixelPositionVal = divStyle.top !== "1%";

		// Support: Android 4.0 - 4.3 only, Firefox <=3 - 44
		reliableMarginLeftVal = roundPixelMeasures( divStyle.marginLeft ) === 12;

		// Support: Android 4.0 - 4.3 only, Safari <=9.1 - 10.1, iOS <=7.0 - 9.3
		// Some styles come back with percentage values, even though they shouldn't
		div.style.right = "60%";
		pixelBoxStylesVal = roundPixelMeasures( divStyle.right ) === 36;

		// Support: IE 9 - 11 only
		// Detect misreporting of content dimensions for box-sizing:border-box elements
		boxSizingReliableVal = roundPixelMeasures( divStyle.width ) === 36;

		// Support: IE 9 only
		// Detect overflow:scroll screwiness (gh-3699)
		// Support: Chrome <=64
		// Don't get tricked when zoom affects offsetWidth (gh-4029)
		div.style.position = "absolute";
		scrollboxSizeVal = roundPixelMeasures( div.offsetWidth / 3 ) === 12;

		documentElement.removeChild( container );

		// Nullify the div so it wouldn't be stored in the memory and
		// it will also be a sign that checks already performed
		div = null;
	}

	function roundPixelMeasures( measure ) {
		return Math.round( parseFloat( measure ) );
	}

	var pixelPositionVal, boxSizingReliableVal, scrollboxSizeVal, pixelBoxStylesVal,
		reliableTrDimensionsVal, reliableMarginLeftVal,
		container = document.createElement( "div" ),
		div = document.createElement( "div" );

	// Finish early in limited (non-browser) environments
	if ( !div.style ) {
		return;
	}

	// Support: IE <=9 - 11 only
	// Style of cloned element affects source element cloned (#8908)
	div.style.backgroundClip = "content-box";
	div.cloneNode( true ).style.backgroundClip = "";
	support.clearCloneStyle = div.style.backgroundClip === "content-box";

	jQuery.extend( support, {
		boxSizingReliable: function() {
			computeStyleTests();
			return boxSizingReliableVal;
		},
		pixelBoxStyles: function() {
			computeStyleTests();
			return pixelBoxStylesVal;
		},
		pixelPosition: function() {
			computeStyleTests();
			return pixelPositionVal;
		},
		reliableMarginLeft: function() {
			computeStyleTests();
			return reliableMarginLeftVal;
		},
		scrollboxSize: function() {
			computeStyleTests();
			return scrollboxSizeVal;
		},

		// Support: IE 9 - 11+, Edge 15 - 18+
		// IE/Edge misreport `getComputedStyle` of table rows with width/height
		// set in CSS while `offset*` properties report correct values.
		// Behavior in IE 9 is more subtle than in newer versions & it passes
		// some versions of this test; make sure not to make it pass there!
		//
		// Support: Firefox 70+
		// Only Firefox includes border widths
		// in computed dimensions. (gh-4529)
		reliableTrDimensions: function() {
			var table, tr, trChild, trStyle;
			if ( reliableTrDimensionsVal == null ) {
				table = document.createElement( "table" );
				tr = document.createElement( "tr" );
				trChild = document.createElement( "div" );

				table.style.cssText = "position:absolute;left:-11111px;border-collapse:separate";
				tr.style.cssText = "border:1px solid";

				// Support: Chrome 86+
				// Height set through cssText does not get applied.
				// Computed height then comes back as 0.
				tr.style.height = "1px";
				trChild.style.height = "9px";

				// Support: Android 8 Chrome 86+
				// In our bodyBackground.html iframe,
				// display for all div elements is set to "inline",
				// which causes a problem only in Android 8 Chrome 86.
				// Ensuring the div is display: block
				// gets around this issue.
				trChild.style.display = "block";

				documentElement
					.appendChild( table )
					.appendChild( tr )
					.appendChild( trChild );

				trStyle = window.getComputedStyle( tr );
				reliableTrDimensionsVal = ( parseInt( trStyle.height, 10 ) +
					parseInt( trStyle.borderTopWidth, 10 ) +
					parseInt( trStyle.borderBottomWidth, 10 ) ) === tr.offsetHeight;

				documentElement.removeChild( table );
			}
			return reliableTrDimensionsVal;
		}
	} );
} )();


function curCSS( elem, name, computed ) {
	var width, minWidth, maxWidth, ret,

		// Support: Firefox 51+
		// Retrieving style before computed somehow
		// fixes an issue with getting wrong values
		// on detached elements
		style = elem.style;

	computed = computed || getStyles( elem );

	// getPropertyValue is needed for:
	//   .css('filter') (IE 9 only, #12537)
	//   .css('--customProperty) (#3144)
	if ( computed ) {
		ret = computed.getPropertyValue( name ) || computed[ name ];

		if ( ret === "" && !isAttached( elem ) ) {
			ret = jQuery.style( elem, name );
		}

		// A tribute to the "awesome hack by Dean Edwards"
		// Android Browser returns percentage for some values,
		// but width seems to be reliably pixels.
		// This is against the CSSOM draft spec:
		// https://drafts.csswg.org/cssom/#resolved-values
		if ( !support.pixelBoxStyles() && rnumnonpx.test( ret ) && rboxStyle.test( name ) ) {

			// Remember the original values
			width = style.width;
			minWidth = style.minWidth;
			maxWidth = style.maxWidth;

			// Put in the new values to get a computed value out
			style.minWidth = style.maxWidth = style.width = ret;
			ret = computed.width;

			// Revert the changed values
			style.width = width;
			style.minWidth = minWidth;
			style.maxWidth = maxWidth;
		}
	}

	return ret !== undefined ?

		// Support: IE <=9 - 11 only
		// IE returns zIndex value as an integer.
		ret + "" :
		ret;
}


function addGetHookIf( conditionFn, hookFn ) {

	// Define the hook, we'll check on the first run if it's really needed.
	return {
		get: function() {
			if ( conditionFn() ) {

				// Hook not needed (or it's not possible to use it due
				// to missing dependency), remove it.
				delete this.get;
				return;
			}

			// Hook needed; redefine it so that the support test is not executed again.
			return ( this.get = hookFn ).apply( this, arguments );
		}
	};
}


var cssPrefixes = [ "Webkit", "Moz", "ms" ],
	emptyStyle = document.createElement( "div" ).style,
	vendorProps = {};

// Return a vendor-prefixed property or undefined
function vendorPropName( name ) {

	// Check for vendor prefixed names
	var capName = name[ 0 ].toUpperCase() + name.slice( 1 ),
		i = cssPrefixes.length;

	while ( i-- ) {
		name = cssPrefixes[ i ] + capName;
		if ( name in emptyStyle ) {
			return name;
		}
	}
}

// Return a potentially-mapped jQuery.cssProps or vendor prefixed property
function finalPropName( name ) {
	var final = jQuery.cssProps[ name ] || vendorProps[ name ];

	if ( final ) {
		return final;
	}
	if ( name in emptyStyle ) {
		return name;
	}
	return vendorProps[ name ] = vendorPropName( name ) || name;
}


var

	// Swappable if display is none or starts with table
	// except "table", "table-cell", or "table-caption"
	// See here for display values: https://developer.mozilla.org/en-US/docs/CSS/display
	rdisplayswap = /^(none|table(?!-c[ea]).+)/,
	rcustomProp = /^--/,
	cssShow = { position: "absolute", visibility: "hidden", display: "block" },
	cssNormalTransform = {
		letterSpacing: "0",
		fontWeight: "400"
	};

function setPositiveNumber( _elem, value, subtract ) {

	// Any relative (+/-) values have already been
	// normalized at this point
	var matches = rcssNum.exec( value );
	return matches ?

		// Guard against undefined "subtract", e.g., when used as in cssHooks
		Math.max( 0, matches[ 2 ] - ( subtract || 0 ) ) + ( matches[ 3 ] || "px" ) :
		value;
}

function boxModelAdjustment( elem, dimension, box, isBorderBox, styles, computedVal ) {
	var i = dimension === "width" ? 1 : 0,
		extra = 0,
		delta = 0;

	// Adjustment may not be necessary
	if ( box === ( isBorderBox ? "border" : "content" ) ) {
		return 0;
	}

	for ( ; i < 4; i += 2 ) {

		// Both box models exclude margin
		if ( box === "margin" ) {
			delta += jQuery.css( elem, box + cssExpand[ i ], true, styles );
		}

		// If we get here with a content-box, we're seeking "padding" or "border" or "margin"
		if ( !isBorderBox ) {

			// Add padding
			delta += jQuery.css( elem, "padding" + cssExpand[ i ], true, styles );

			// For "border" or "margin", add border
			if ( box !== "padding" ) {
				delta += jQuery.css( elem, "border" + cssExpand[ i ] + "Width", true, styles );

			// But still keep track of it otherwise
			} else {
				extra += jQuery.css( elem, "border" + cssExpand[ i ] + "Width", true, styles );
			}

		// If we get here with a border-box (content + padding + border), we're seeking "content" or
		// "padding" or "margin"
		} else {

			// For "content", subtract padding
			if ( box === "content" ) {
				delta -= jQuery.css( elem, "padding" + cssExpand[ i ], true, styles );
			}

			// For "content" or "padding", subtract border
			if ( box !== "margin" ) {
				delta -= jQuery.css( elem, "border" + cssExpand[ i ] + "Width", true, styles );
			}
		}
	}

	// Account for positive content-box scroll gutter when requested by providing computedVal
	if ( !isBorderBox && computedVal >= 0 ) {

		// offsetWidth/offsetHeight is a rounded sum of content, padding, scroll gutter, and border
		// Assuming integer scroll gutter, subtract the rest and round down
		delta += Math.max( 0, Math.ceil(
			elem[ "offset" + dimension[ 0 ].toUpperCase() + dimension.slice( 1 ) ] -
			computedVal -
			delta -
			extra -
			0.5

		// If offsetWidth/offsetHeight is unknown, then we can't determine content-box scroll gutter
		// Use an explicit zero to avoid NaN (gh-3964)
		) ) || 0;
	}

	return delta;
}

function getWidthOrHeight( elem, dimension, extra ) {

	// Start with computed style
	var styles = getStyles( elem ),

		// To avoid forcing a reflow, only fetch boxSizing if we need it (gh-4322).
		// Fake content-box until we know it's needed to know the true value.
		boxSizingNeeded = !support.boxSizingReliable() || extra,
		isBorderBox = boxSizingNeeded &&
			jQuery.css( elem, "boxSizing", false, styles ) === "border-box",
		valueIsBorderBox = isBorderBox,

		val = curCSS( elem, dimension, styles ),
		offsetProp = "offset" + dimension[ 0 ].toUpperCase() + dimension.slice( 1 );

	// Support: Firefox <=54
	// Return a confounding non-pixel value or feign ignorance, as appropriate.
	if ( rnumnonpx.test( val ) ) {
		if ( !extra ) {
			return val;
		}
		val = "auto";
	}


	// Support: IE 9 - 11 only
	// Use offsetWidth/offsetHeight for when box sizing is unreliable.
	// In those cases, the computed value can be trusted to be border-box.
	if ( ( !support.boxSizingReliable() && isBorderBox ||

		// Support: IE 10 - 11+, Edge 15 - 18+
		// IE/Edge misreport `getComputedStyle` of table rows with width/height
		// set in CSS while `offset*` properties report correct values.
		// Interestingly, in some cases IE 9 doesn't suffer from this issue.
		!support.reliableTrDimensions() && nodeName( elem, "tr" ) ||

		// Fall back to offsetWidth/offsetHeight when value is "auto"
		// This happens for inline elements with no explicit setting (gh-3571)
		val === "auto" ||

		// Support: Android <=4.1 - 4.3 only
		// Also use offsetWidth/offsetHeight for misreported inline dimensions (gh-3602)
		!parseFloat( val ) && jQuery.css( elem, "display", false, styles ) === "inline" ) &&

		// Make sure the element is visible & connected
		elem.getClientRects().length ) {

		isBorderBox = jQuery.css( elem, "boxSizing", false, styles ) === "border-box";

		// Where available, offsetWidth/offsetHeight approximate border box dimensions.
		// Where not available (e.g., SVG), assume unreliable box-sizing and interpret the
		// retrieved value as a content box dimension.
		valueIsBorderBox = offsetProp in elem;
		if ( valueIsBorderBox ) {
			val = elem[ offsetProp ];
		}
	}

	// Normalize "" and auto
	val = parseFloat( val ) || 0;

	// Adjust for the element's box model
	return ( val +
		boxModelAdjustment(
			elem,
			dimension,
			extra || ( isBorderBox ? "border" : "content" ),
			valueIsBorderBox,
			styles,

			// Provide the current computed size to request scroll gutter calculation (gh-3589)
			val
		)
	) + "px";
}

jQuery.extend( {

	// Add in style property hooks for overriding the default
	// behavior of getting and setting a style property
	cssHooks: {
		opacity: {
			get: function( elem, computed ) {
				if ( computed ) {

					// We should always get a number back from opacity
					var ret = curCSS( elem, "opacity" );
					return ret === "" ? "1" : ret;
				}
			}
		}
	},

	// Don't automatically add "px" to these possibly-unitless properties
	cssNumber: {
		"animationIterationCount": true,
		"columnCount": true,
		"fillOpacity": true,
		"flexGrow": true,
		"flexShrink": true,
		"fontWeight": true,
		"gridArea": true,
		"gridColumn": true,
		"gridColumnEnd": true,
		"gridColumnStart": true,
		"gridRow": true,
		"gridRowEnd": true,
		"gridRowStart": true,
		"lineHeight": true,
		"opacity": true,
		"order": true,
		"orphans": true,
		"widows": true,
		"zIndex": true,
		"zoom": true
	},

	// Add in properties whose names you wish to fix before
	// setting or getting the value
	cssProps: {},

	// Get and set the style property on a DOM Node
	style: function( elem, name, value, extra ) {

		// Don't set styles on text and comment nodes
		if ( !elem || elem.nodeType === 3 || elem.nodeType === 8 || !elem.style ) {
			return;
		}

		// Make sure that we're working with the right name
		var ret, type, hooks,
			origName = camelCase( name ),
			isCustomProp = rcustomProp.test( name ),
			style = elem.style;

		// Make sure that we're working with the right name. We don't
		// want to query the value if it is a CSS custom property
		// since they are user-defined.
		if ( !isCustomProp ) {
			name = finalPropName( origName );
		}

		// Gets hook for the prefixed version, then unprefixed version
		hooks = jQuery.cssHooks[ name ] || jQuery.cssHooks[ origName ];

		// Check if we're setting a value
		if ( value !== undefined ) {
			type = typeof value;

			// Convert "+=" or "-=" to relative numbers (#7345)
			if ( type === "string" && ( ret = rcssNum.exec( value ) ) && ret[ 1 ] ) {
				value = adjustCSS( elem, name, ret );

				// Fixes bug #9237
				type = "number";
			}

			// Make sure that null and NaN values aren't set (#7116)
			if ( value == null || value !== value ) {
				return;
			}

			// If a number was passed in, add the unit (except for certain CSS properties)
			// The isCustomProp check can be removed in jQuery 4.0 when we only auto-append
			// "px" to a few hardcoded values.
			if ( type === "number" && !isCustomProp ) {
				value += ret && ret[ 3 ] || ( jQuery.cssNumber[ origName ] ? "" : "px" );
			}

			// background-* props affect original clone's values
			if ( !support.clearCloneStyle && value === "" && name.indexOf( "background" ) === 0 ) {
				style[ name ] = "inherit";
			}

			// If a hook was provided, use that value, otherwise just set the specified value
			if ( !hooks || !( "set" in hooks ) ||
				( value = hooks.set( elem, value, extra ) ) !== undefined ) {

				if ( isCustomProp ) {
					style.setProperty( name, value );
				} else {
					style[ name ] = value;
				}
			}

		} else {

			// If a hook was provided get the non-computed value from there
			if ( hooks && "get" in hooks &&
				( ret = hooks.get( elem, false, extra ) ) !== undefined ) {

				return ret;
			}

			// Otherwise just get the value from the style object
			return style[ name ];
		}
	},

	css: function( elem, name, extra, styles ) {
		var val, num, hooks,
			origName = camelCase( name ),
			isCustomProp = rcustomProp.test( name );

		// Make sure that we're working with the right name. We don't
		// want to modify the value if it is a CSS custom property
		// since they are user-defined.
		if ( !isCustomProp ) {
			name = finalPropName( origName );
		}

		// Try prefixed name followed by the unprefixed name
		hooks = jQuery.cssHooks[ name ] || jQuery.cssHooks[ origName ];

		// If a hook was provided get the computed value from there
		if ( hooks && "get" in hooks ) {
			val = hooks.get( elem, true, extra );
		}

		// Otherwise, if a way to get the computed value exists, use that
		if ( val === undefined ) {
			val = curCSS( elem, name, styles );
		}

		// Convert "normal" to computed value
		if ( val === "normal" && name in cssNormalTransform ) {
			val = cssNormalTransform[ name ];
		}

		// Make numeric if forced or a qualifier was provided and val looks numeric
		if ( extra === "" || extra ) {
			num = parseFloat( val );
			return extra === true || isFinite( num ) ? num || 0 : val;
		}

		return val;
	}
} );

jQuery.each( [ "height", "width" ], function( _i, dimension ) {
	jQuery.cssHooks[ dimension ] = {
		get: function( elem, computed, extra ) {
			if ( computed ) {

				// Certain elements can have dimension info if we invisibly show them
				// but it must have a current display style that would benefit
				return rdisplayswap.test( jQuery.css( elem, "display" ) ) &&

					// Support: Safari 8+
					// Table columns in Safari have non-zero offsetWidth & zero
					// getBoundingClientRect().width unless display is changed.
					// Support: IE <=11 only
					// Running getBoundingClientRect on a disconnected node
					// in IE throws an error.
					( !elem.getClientRects().length || !elem.getBoundingClientRect().width ) ?
					swap( elem, cssShow, function() {
						return getWidthOrHeight( elem, dimension, extra );
					} ) :
					getWidthOrHeight( elem, dimension, extra );
			}
		},

		set: function( elem, value, extra ) {
			var matches,
				styles = getStyles( elem ),

				// Only read styles.position if the test has a chance to fail
				// to avoid forcing a reflow.
				scrollboxSizeBuggy = !support.scrollboxSize() &&
					styles.position === "absolute",

				// To avoid forcing a reflow, only fetch boxSizing if we need it (gh-3991)
				boxSizingNeeded = scrollboxSizeBuggy || extra,
				isBorderBox = boxSizingNeeded &&
					jQuery.css( elem, "boxSizing", false, styles ) === "border-box",
				subtract = extra ?
					boxModelAdjustment(
						elem,
						dimension,
						extra,
						isBorderBox,
						styles
					) :
					0;

			// Account for unreliable border-box dimensions by comparing offset* to computed and
			// faking a content-box to get border and padding (gh-3699)
			if ( isBorderBox && scrollboxSizeBuggy ) {
				subtract -= Math.ceil(
					elem[ "offset" + dimension[ 0 ].toUpperCase() + dimension.slice( 1 ) ] -
					parseFloat( styles[ dimension ] ) -
					boxModelAdjustment( elem, dimension, "border", false, styles ) -
					0.5
				);
			}

			// Convert to pixels if value adjustment is needed
			if ( subtract && ( matches = rcssNum.exec( value ) ) &&
				( matches[ 3 ] || "px" ) !== "px" ) {

				elem.style[ dimension ] = value;
				value = jQuery.css( elem, dimension );
			}

			return setPositiveNumber( elem, value, subtract );
		}
	};
} );

jQuery.cssHooks.marginLeft = addGetHookIf( support.reliableMarginLeft,
	function( elem, computed ) {
		if ( computed ) {
			return ( parseFloat( curCSS( elem, "marginLeft" ) ) ||
				elem.getBoundingClientRect().left -
					swap( elem, { marginLeft: 0 }, function() {
						return elem.getBoundingClientRect().left;
					} )
			) + "px";
		}
	}
);

// These hooks are used by animate to expand properties
jQuery.each( {
	margin: "",
	padding: "",
	border: "Width"
}, function( prefix, suffix ) {
	jQuery.cssHooks[ prefix + suffix ] = {
		expand: function( value ) {
			var i = 0,
				expanded = {},

				// Assumes a single number if not a string
				parts = typeof value === "string" ? value.split( " " ) : [ value ];

			for ( ; i < 4; i++ ) {
				expanded[ prefix + cssExpand[ i ] + suffix ] =
					parts[ i ] || parts[ i - 2 ] || parts[ 0 ];
			}

			return expanded;
		}
	};

	if ( prefix !== "margin" ) {
		jQuery.cssHooks[ prefix + suffix ].set = setPositiveNumber;
	}
} );

jQuery.fn.extend( {
	css: function( name, value ) {
		return access( this, function( elem, name, value ) {
			var styles, len,
				map = {},
				i = 0;

			if ( Array.isArray( name ) ) {
				styles = getStyles( elem );
				len = name.length;

				for ( ; i < len; i++ ) {
					map[ name[ i ] ] = jQuery.css( elem, name[ i ], false, styles );
				}

				return map;
			}

			return value !== undefined ?
				jQuery.style( elem, name, value ) :
				jQuery.css( elem, name );
		}, name, value, arguments.length > 1 );
	}
} );


function Tween( elem, options, prop, end, easing ) {
	return new Tween.prototype.init( elem, options, prop, end, easing );
}
jQuery.Tween = Tween;

Tween.prototype = {
	constructor: Tween,
	init: function( elem, options, prop, end, easing, unit ) {
		this.elem = elem;
		this.prop = prop;
		this.easing = easing || jQuery.easing._default;
		this.options = options;
		this.start = this.now = this.cur();
		this.end = end;
		this.unit = unit || ( jQuery.cssNumber[ prop ] ? "" : "px" );
	},
	cur: function() {
		var hooks = Tween.propHooks[ this.prop ];

		return hooks && hooks.get ?
			hooks.get( this ) :
			Tween.propHooks._default.get( this );
	},
	run: function( percent ) {
		var eased,
			hooks = Tween.propHooks[ this.prop ];

		if ( this.options.duration ) {
			this.pos = eased = jQuery.easing[ this.easing ](
				percent, this.options.duration * percent, 0, 1, this.options.duration
			);
		} else {
			this.pos = eased = percent;
		}
		this.now = ( this.end - this.start ) * eased + this.start;

		if ( this.options.step ) {
			this.options.step.call( this.elem, this.now, this );
		}

		if ( hooks && hooks.set ) {
			hooks.set( this );
		} else {
			Tween.propHooks._default.set( this );
		}
		return this;
	}
};

Tween.prototype.init.prototype = Tween.prototype;

Tween.propHooks = {
	_default: {
		get: function( tween ) {
			var result;

			// Use a property on the element directly when it is not a DOM element,
			// or when there is no matching style property that exists.
			if ( tween.elem.nodeType !== 1 ||
				tween.elem[ tween.prop ] != null && tween.elem.style[ tween.prop ] == null ) {
				return tween.elem[ tween.prop ];
			}

			// Passing an empty string as a 3rd parameter to .css will automatically
			// attempt a parseFloat and fallback to a string if the parse fails.
			// Simple values such as "10px" are parsed to Float;
			// complex values such as "rotate(1rad)" are returned as-is.
			result = jQuery.css( tween.elem, tween.prop, "" );

			// Empty strings, null, undefined and "auto" are converted to 0.
			return !result || result === "auto" ? 0 : result;
		},
		set: function( tween ) {

			// Use step hook for back compat.
			// Use cssHook if its there.
			// Use .style if available and use plain properties where available.
			if ( jQuery.fx.step[ tween.prop ] ) {
				jQuery.fx.step[ tween.prop ]( tween );
			} else if ( tween.elem.nodeType === 1 && (
				jQuery.cssHooks[ tween.prop ] ||
					tween.elem.style[ finalPropName( tween.prop ) ] != null ) ) {
				jQuery.style( tween.elem, tween.prop, tween.now + tween.unit );
			} else {
				tween.elem[ tween.prop ] = tween.now;
			}
		}
	}
};

// Support: IE <=9 only
// Panic based approach to setting things on disconnected nodes
Tween.propHooks.scrollTop = Tween.propHooks.scrollLeft = {
	set: function( tween ) {
		if ( tween.elem.nodeType && tween.elem.parentNode ) {
			tween.elem[ tween.prop ] = tween.now;
		}
	}
};

jQuery.easing = {
	linear: function( p ) {
		return p;
	},
	swing: function( p ) {
		return 0.5 - Math.cos( p * Math.PI ) / 2;
	},
	_default: "swing"
};

jQuery.fx = Tween.prototype.init;

// Back compat <1.8 extension point
jQuery.fx.step = {};




var
	fxNow, inProgress,
	rfxtypes = /^(?:toggle|show|hide)$/,
	rrun = /queueHooks$/;

function schedule() {
	if ( inProgress ) {
		if ( document.hidden === false && window.requestAnimationFrame ) {
			window.requestAnimationFrame( schedule );
		} else {
			window.setTimeout( schedule, jQuery.fx.interval );
		}

		jQuery.fx.tick();
	}
}

// Animations created synchronously will run synchronously
function createFxNow() {
	window.setTimeout( function() {
		fxNow = undefined;
	} );
	return ( fxNow = Date.now() );
}

// Generate parameters to create a standard animation
function genFx( type, includeWidth ) {
	var which,
		i = 0,
		attrs = { height: type };

	// If we include width, step value is 1 to do all cssExpand values,
	// otherwise step value is 2 to skip over Left and Right
	includeWidth = includeWidth ? 1 : 0;
	for ( ; i < 4; i += 2 - includeWidth ) {
		which = cssExpand[ i ];
		attrs[ "margin" + which ] = attrs[ "padding" + which ] = type;
	}

	if ( includeWidth ) {
		attrs.opacity = attrs.width = type;
	}

	return attrs;
}

function createTween( value, prop, animation ) {
	var tween,
		collection = ( Animation.tweeners[ prop ] || [] ).concat( Animation.tweeners[ "*" ] ),
		index = 0,
		length = collection.length;
	for ( ; index < length; index++ ) {
		if ( ( tween = collection[ index ].call( animation, prop, value ) ) ) {

			// We're done with this property
			return tween;
		}
	}
}

function defaultPrefilter( elem, props, opts ) {
	var prop, value, toggle, hooks, oldfire, propTween, restoreDisplay, display,
		isBox = "width" in props || "height" in props,
		anim = this,
		orig = {},
		style = elem.style,
		hidden = elem.nodeType && isHiddenWithinTree( elem ),
		dataShow = dataPriv.get( elem, "fxshow" );

	// Queue-skipping animations hijack the fx hooks
	if ( !opts.queue ) {
		hooks = jQuery._queueHooks( elem, "fx" );
		if ( hooks.unqueued == null ) {
			hooks.unqueued = 0;
			oldfire = hooks.empty.fire;
			hooks.empty.fire = function() {
				if ( !hooks.unqueued ) {
					oldfire();
				}
			};
		}
		hooks.unqueued++;

		anim.always( function() {

			// Ensure the complete handler is called before this completes
			anim.always( function() {
				hooks.unqueued--;
				if ( !jQuery.queue( elem, "fx" ).length ) {
					hooks.empty.fire();
				}
			} );
		} );
	}

	// Detect show/hide animations
	for ( prop in props ) {
		value = props[ prop ];
		if ( rfxtypes.test( value ) ) {
			delete props[ prop ];
			toggle = toggle || value === "toggle";
			if ( value === ( hidden ? "hide" : "show" ) ) {

				// Pretend to be hidden if this is a "show" and
				// there is still data from a stopped show/hide
				if ( value === "show" && dataShow && dataShow[ prop ] !== undefined ) {
					hidden = true;

				// Ignore all other no-op show/hide data
				} else {
					continue;
				}
			}
			orig[ prop ] = dataShow && dataShow[ prop ] || jQuery.style( elem, prop );
		}
	}

	// Bail out if this is a no-op like .hide().hide()
	propTween = !jQuery.isEmptyObject( props );
	if ( !propTween && jQuery.isEmptyObject( orig ) ) {
		return;
	}

	// Restrict "overflow" and "display" styles during box animations
	if ( isBox && elem.nodeType === 1 ) {

		// Support: IE <=9 - 11, Edge 12 - 15
		// Record all 3 overflow attributes because IE does not infer the shorthand
		// from identically-valued overflowX and overflowY and Edge just mirrors
		// the overflowX value there.
		opts.overflow = [ style.overflow, style.overflowX, style.overflowY ];

		// Identify a display type, preferring old show/hide data over the CSS cascade
		restoreDisplay = dataShow && dataShow.display;
		if ( restoreDisplay == null ) {
			restoreDisplay = dataPriv.get( elem, "display" );
		}
		display = jQuery.css( elem, "display" );
		if ( display === "none" ) {
			if ( restoreDisplay ) {
				display = restoreDisplay;
			} else {

				// Get nonempty value(s) by temporarily forcing visibility
				showHide( [ elem ], true );
				restoreDisplay = elem.style.display || restoreDisplay;
				display = jQuery.css( elem, "display" );
				showHide( [ elem ] );
			}
		}

		// Animate inline elements as inline-block
		if ( display === "inline" || display === "inline-block" && restoreDisplay != null ) {
			if ( jQuery.css( elem, "float" ) === "none" ) {

				// Restore the original display value at the end of pure show/hide animations
				if ( !propTween ) {
					anim.done( function() {
						style.display = restoreDisplay;
					} );
					if ( restoreDisplay == null ) {
						display = style.display;
						restoreDisplay = display === "none" ? "" : display;
					}
				}
				style.display = "inline-block";
			}
		}
	}

	if ( opts.overflow ) {
		style.overflow = "hidden";
		anim.always( function() {
			style.overflow = opts.overflow[ 0 ];
			style.overflowX = opts.overflow[ 1 ];
			style.overflowY = opts.overflow[ 2 ];
		} );
	}

	// Implement show/hide animations
	propTween = false;
	for ( prop in orig ) {

		// General show/hide setup for this element animation
		if ( !propTween ) {
			if ( dataShow ) {
				if ( "hidden" in dataShow ) {
					hidden = dataShow.hidden;
				}
			} else {
				dataShow = dataPriv.access( elem, "fxshow", { display: restoreDisplay } );
			}

			// Store hidden/visible for toggle so `.stop().toggle()` "reverses"
			if ( toggle ) {
				dataShow.hidden = !hidden;
			}

			// Show elements before animating them
			if ( hidden ) {
				showHide( [ elem ], true );
			}

			/* eslint-disable no-loop-func */

			anim.done( function() {

				/* eslint-enable no-loop-func */

				// The final step of a "hide" animation is actually hiding the element
				if ( !hidden ) {
					showHide( [ elem ] );
				}
				dataPriv.remove( elem, "fxshow" );
				for ( prop in orig ) {
					jQuery.style( elem, prop, orig[ prop ] );
				}
			} );
		}

		// Per-property setup
		propTween = createTween( hidden ? dataShow[ prop ] : 0, prop, anim );
		if ( !( prop in dataShow ) ) {
			dataShow[ prop ] = propTween.start;
			if ( hidden ) {
				propTween.end = propTween.start;
				propTween.start = 0;
			}
		}
	}
}

function propFilter( props, specialEasing ) {
	var index, name, easing, value, hooks;

	// camelCase, specialEasing and expand cssHook pass
	for ( index in props ) {
		name = camelCase( index );
		easing = specialEasing[ name ];
		value = props[ index ];
		if ( Array.isArray( value ) ) {
			easing = value[ 1 ];
			value = props[ index ] = value[ 0 ];
		}

		if ( index !== name ) {
			props[ name ] = value;
			delete props[ index ];
		}

		hooks = jQuery.cssHooks[ name ];
		if ( hooks && "expand" in hooks ) {
			value = hooks.expand( value );
			delete props[ name ];

			// Not quite $.extend, this won't overwrite existing keys.
			// Reusing 'index' because we have the correct "name"
			for ( index in value ) {
				if ( !( index in props ) ) {
					props[ index ] = value[ index ];
					specialEasing[ index ] = easing;
				}
			}
		} else {
			specialEasing[ name ] = easing;
		}
	}
}

function Animation( elem, properties, options ) {
	var result,
		stopped,
		index = 0,
		length = Animation.prefilters.length,
		deferred = jQuery.Deferred().always( function() {

			// Don't match elem in the :animated selector
			delete tick.elem;
		} ),
		tick = function() {
			if ( stopped ) {
				return false;
			}
			var currentTime = fxNow || createFxNow(),
				remaining = Math.max( 0, animation.startTime + animation.duration - currentTime ),

				// Support: Android 2.3 only
				// Archaic crash bug won't allow us to use `1 - ( 0.5 || 0 )` (#12497)
				temp = remaining / animation.duration || 0,
				percent = 1 - temp,
				index = 0,
				length = animation.tweens.length;

			for ( ; index < length; index++ ) {
				animation.tweens[ index ].run( percent );
			}

			deferred.notifyWith( elem, [ animation, percent, remaining ] );

			// If there's more to do, yield
			if ( percent < 1 && length ) {
				return remaining;
			}

			// If this was an empty animation, synthesize a final progress notification
			if ( !length ) {
				deferred.notifyWith( elem, [ animation, 1, 0 ] );
			}

			// Resolve the animation and report its conclusion
			deferred.resolveWith( elem, [ animation ] );
			return false;
		},
		animation = deferred.promise( {
			elem: elem,
			props: jQuery.extend( {}, properties ),
			opts: jQuery.extend( true, {
				specialEasing: {},
				easing: jQuery.easing._default
			}, options ),
			originalProperties: properties,
			originalOptions: options,
			startTime: fxNow || createFxNow(),
			duration: options.duration,
			tweens: [],
			createTween: function( prop, end ) {
				var tween = jQuery.Tween( elem, animation.opts, prop, end,
					animation.opts.specialEasing[ prop ] || animation.opts.easing );
				animation.tweens.push( tween );
				return tween;
			},
			stop: function( gotoEnd ) {
				var index = 0,

					// If we are going to the end, we want to run all the tweens
					// otherwise we skip this part
					length = gotoEnd ? animation.tweens.length : 0;
				if ( stopped ) {
					return this;
				}
				stopped = true;
				for ( ; index < length; index++ ) {
					animation.tweens[ index ].run( 1 );
				}

				// Resolve when we played the last frame; otherwise, reject
				if ( gotoEnd ) {
					deferred.notifyWith( elem, [ animation, 1, 0 ] );
					deferred.resolveWith( elem, [ animation, gotoEnd ] );
				} else {
					deferred.rejectWith( elem, [ animation, gotoEnd ] );
				}
				return this;
			}
		} ),
		props = animation.props;

	propFilter( props, animation.opts.specialEasing );

	for ( ; index < length; index++ ) {
		result = Animation.prefilters[ index ].call( animation, elem, props, animation.opts );
		if ( result ) {
			if ( isFunction( result.stop ) ) {
				jQuery._queueHooks( animation.elem, animation.opts.queue ).stop =
					result.stop.bind( result );
			}
			return result;
		}
	}

	jQuery.map( props, createTween, animation );

	if ( isFunction( animation.opts.start ) ) {
		animation.opts.start.call( elem, animation );
	}

	// Attach callbacks from options
	animation
		.progress( animation.opts.progress )
		.done( animation.opts.done, animation.opts.complete )
		.fail( animation.opts.fail )
		.always( animation.opts.always );

	jQuery.fx.timer(
		jQuery.extend( tick, {
			elem: elem,
			anim: animation,
			queue: animation.opts.queue
		} )
	);

	return animation;
}

jQuery.Animation = jQuery.extend( Animation, {

	tweeners: {
		"*": [ function( prop, value ) {
			var tween = this.createTween( prop, value );
			adjustCSS( tween.elem, prop, rcssNum.exec( value ), tween );
			return tween;
		} ]
	},

	tweener: function( props, callback ) {
		if ( isFunction( props ) ) {
			callback = props;
			props = [ "*" ];
		} else {
			props = props.match( rnothtmlwhite );
		}

		var prop,
			index = 0,
			length = props.length;

		for ( ; index < length; index++ ) {
			prop = props[ index ];
			Animation.tweeners[ prop ] = Animation.tweeners[ prop ] || [];
			Animation.tweeners[ prop ].unshift( callback );
		}
	},

	prefilters: [ defaultPrefilter ],

	prefilter: function( callback, prepend ) {
		if ( prepend ) {
			Animation.prefilters.unshift( callback );
		} else {
			Animation.prefilters.push( callback );
		}
	}
} );

jQuery.speed = function( speed, easing, fn ) {
	var opt = speed && typeof speed === "object" ? jQuery.extend( {}, speed ) : {
		complete: fn || !fn && easing ||
			isFunction( speed ) && speed,
		duration: speed,
		easing: fn && easing || easing && !isFunction( easing ) && easing
	};

	// Go to the end state if fx are off
	if ( jQuery.fx.off ) {
		opt.duration = 0;

	} else {
		if ( typeof opt.duration !== "number" ) {
			if ( opt.duration in jQuery.fx.speeds ) {
				opt.duration = jQuery.fx.speeds[ opt.duration ];

			} else {
				opt.duration = jQuery.fx.speeds._default;
			}
		}
	}

	// Normalize opt.queue - true/undefined/null -> "fx"
	if ( opt.queue == null || opt.queue === true ) {
		opt.queue = "fx";
	}

	// Queueing
	opt.old = opt.complete;

	opt.complete = function() {
		if ( isFunction( opt.old ) ) {
			opt.old.call( this );
		}

		if ( opt.queue ) {
			jQuery.dequeue( this, opt.queue );
		}
	};

	return opt;
};

jQuery.fn.extend( {
	fadeTo: function( speed, to, easing, callback ) {

		// Show any hidden elements after setting opacity to 0
		return this.filter( isHiddenWithinTree ).css( "opacity", 0 ).show()

			// Animate to the value specified
			.end().animate( { opacity: to }, speed, easing, callback );
	},
	animate: function( prop, speed, easing, callback ) {
		var empty = jQuery.isEmptyObject( prop ),
			optall = jQuery.speed( speed, easing, callback ),
			doAnimation = function() {

				// Operate on a copy of prop so per-property easing won't be lost
				var anim = Animation( this, jQuery.extend( {}, prop ), optall );

				// Empty animations, or finishing resolves immediately
				if ( empty || dataPriv.get( this, "finish" ) ) {
					anim.stop( true );
				}
			};

		doAnimation.finish = doAnimation;

		return empty || optall.queue === false ?
			this.each( doAnimation ) :
			this.queue( optall.queue, doAnimation );
	},
	stop: function( type, clearQueue, gotoEnd ) {
		var stopQueue = function( hooks ) {
			var stop = hooks.stop;
			delete hooks.stop;
			stop( gotoEnd );
		};

		if ( typeof type !== "string" ) {
			gotoEnd = clearQueue;
			clearQueue = type;
			type = undefined;
		}
		if ( clearQueue ) {
			this.queue( type || "fx", [] );
		}

		return this.each( function() {
			var dequeue = true,
				index = type != null && type + "queueHooks",
				timers = jQuery.timers,
				data = dataPriv.get( this );

			if ( index ) {
				if ( data[ index ] && data[ index ].stop ) {
					stopQueue( data[ index ] );
				}
			} else {
				for ( index in data ) {
					if ( data[ index ] && data[ index ].stop && rrun.test( index ) ) {
						stopQueue( data[ index ] );
					}
				}
			}

			for ( index = timers.length; index--; ) {
				if ( timers[ index ].elem === this &&
					( type == null || timers[ index ].queue === type ) ) {

					timers[ index ].anim.stop( gotoEnd );
					dequeue = false;
					timers.splice( index, 1 );
				}
			}

			// Start the next in the queue if the last step wasn't forced.
			// Timers currently will call their complete callbacks, which
			// will dequeue but only if they were gotoEnd.
			if ( dequeue || !gotoEnd ) {
				jQuery.dequeue( this, type );
			}
		} );
	},
	finish: function( type ) {
		if ( type !== false ) {
			type = type || "fx";
		}
		return this.each( function() {
			var index,
				data = dataPriv.get( this ),
				queue = data[ type + "queue" ],
				hooks = data[ type + "queueHooks" ],
				timers = jQuery.timers,
				length = queue ? queue.length : 0;

			// Enable finishing flag on private data
			data.finish = true;

			// Empty the queue first
			jQuery.queue( this, type, [] );

			if ( hooks && hooks.stop ) {
				hooks.stop.call( this, true );
			}

			// Look for any active animations, and finish them
			for ( index = timers.length; index--; ) {
				if ( timers[ index ].elem === this && timers[ index ].queue === type ) {
					timers[ index ].anim.stop( true );
					timers.splice( index, 1 );
				}
			}

			// Look for any animations in the old queue and finish them
			for ( index = 0; index < length; index++ ) {
				if ( queue[ index ] && queue[ index ].finish ) {
					queue[ index ].finish.call( this );
				}
			}

			// Turn off finishing flag
			delete data.finish;
		} );
	}
} );

jQuery.each( [ "toggle", "show", "hide" ], function( _i, name ) {
	var cssFn = jQuery.fn[ name ];
	jQuery.fn[ name ] = function( speed, easing, callback ) {
		return speed == null || typeof speed === "boolean" ?
			cssFn.apply( this, arguments ) :
			this.animate( genFx( name, true ), speed, easing, callback );
	};
} );

// Generate shortcuts for custom animations
jQuery.each( {
	slideDown: genFx( "show" ),
	slideUp: genFx( "hide" ),
	slideToggle: genFx( "toggle" ),
	fadeIn: { opacity: "show" },
	fadeOut: { opacity: "hide" },
	fadeToggle: { opacity: "toggle" }
}, function( name, props ) {
	jQuery.fn[ name ] = function( speed, easing, callback ) {
		return this.animate( props, speed, easing, callback );
	};
} );

jQuery.timers = [];
jQuery.fx.tick = function() {
	var timer,
		i = 0,
		timers = jQuery.timers;

	fxNow = Date.now();

	for ( ; i < timers.length; i++ ) {
		timer = timers[ i ];

		// Run the timer and safely remove it when done (allowing for external removal)
		if ( !timer() && timers[ i ] === timer ) {
			timers.splice( i--, 1 );
		}
	}

	if ( !timers.length ) {
		jQuery.fx.stop();
	}
	fxNow = undefined;
};

jQuery.fx.timer = function( timer ) {
	jQuery.timers.push( timer );
	jQuery.fx.start();
};

jQuery.fx.interval = 13;
jQuery.fx.start = function() {
	if ( inProgress ) {
		return;
	}

	inProgress = true;
	schedule();
};

jQuery.fx.stop = function() {
	inProgress = null;
};

jQuery.fx.speeds = {
	slow: 600,
	fast: 200,

	// Default speed
	_default: 400
};


// Based off of the plugin by Clint Helfers, with permission.
// https://web.archive.org/web/20100324014747/http://blindsignals.com/index.php/2009/07/jquery-delay/
jQuery.fn.delay = function( time, type ) {
	time = jQuery.fx ? jQuery.fx.speeds[ time ] || time : time;
	type = type || "fx";

	return this.queue( type, function( next, hooks ) {
		var timeout = window.setTimeout( next, time );
		hooks.stop = function() {
			window.clearTimeout( timeout );
		};
	} );
};


( function() {
	var input = document.createElement( "input" ),
		select = document.createElement( "select" ),
		opt = select.appendChild( document.createElement( "option" ) );

	input.type = "checkbox";

	// Support: Android <=4.3 only
	// Default value for a checkbox should be "on"
	support.checkOn = input.value !== "";

	// Support: IE <=11 only
	// Must access selectedIndex to make default options select
	support.optSelected = opt.selected;

	// Support: IE <=11 only
	// An input loses its value after becoming a radio
	input = document.createElement( "input" );
	input.value = "t";
	input.type = "radio";
	support.radioValue = input.value === "t";
} )();


var boolHook,
	attrHandle = jQuery.expr.attrHandle;

jQuery.fn.extend( {
	attr: function( name, value ) {
		return access( this, jQuery.attr, name, value, arguments.length > 1 );
	},

	removeAttr: function( name ) {
		return this.each( function() {
			jQuery.removeAttr( this, name );
		} );
	}
} );

jQuery.extend( {
	attr: function( elem, name, value ) {
		var ret, hooks,
			nType = elem.nodeType;

		// Don't get/set attributes on text, comment and attribute nodes
		if ( nType === 3 || nType === 8 || nType === 2 ) {
			return;
		}

		// Fallback to prop when attributes are not supported
		if ( typeof elem.getAttribute === "undefined" ) {
			return jQuery.prop( elem, name, value );
		}

		// Attribute hooks are determined by the lowercase version
		// Grab necessary hook if one is defined
		if ( nType !== 1 || !jQuery.isXMLDoc( elem ) ) {
			hooks = jQuery.attrHooks[ name.toLowerCase() ] ||
				( jQuery.expr.match.bool.test( name ) ? boolHook : undefined );
		}

		if ( value !== undefined ) {
			if ( value === null ) {
				jQuery.removeAttr( elem, name );
				return;
			}

			if ( hooks && "set" in hooks &&
				( ret = hooks.set( elem, value, name ) ) !== undefined ) {
				return ret;
			}

			elem.setAttribute( name, value + "" );
			return value;
		}

		if ( hooks && "get" in hooks && ( ret = hooks.get( elem, name ) ) !== null ) {
			return ret;
		}

		ret = jQuery.find.attr( elem, name );

		// Non-existent attributes return null, we normalize to undefined
		return ret == null ? undefined : ret;
	},

	attrHooks: {
		type: {
			set: function( elem, value ) {
				if ( !support.radioValue && value === "radio" &&
					nodeName( elem, "input" ) ) {
					var val = elem.value;
					elem.setAttribute( "type", value );
					if ( val ) {
						elem.value = val;
					}
					return value;
				}
			}
		}
	},

	removeAttr: function( elem, value ) {
		var name,
			i = 0,

			// Attribute names can contain non-HTML whitespace characters
			// https://html.spec.whatwg.org/multipage/syntax.html#attributes-2
			attrNames = value && value.match( rnothtmlwhite );

		if ( attrNames && elem.nodeType === 1 ) {
			while ( ( name = attrNames[ i++ ] ) ) {
				elem.removeAttribute( name );
			}
		}
	}
} );

// Hooks for boolean attributes
boolHook = {
	set: function( elem, value, name ) {
		if ( value === false ) {

			// Remove boolean attributes when set to false
			jQuery.removeAttr( elem, name );
		} else {
			elem.setAttribute( name, name );
		}
		return name;
	}
};

jQuery.each( jQuery.expr.match.bool.source.match( /\w+/g ), function( _i, name ) {
	var getter = attrHandle[ name ] || jQuery.find.attr;

	attrHandle[ name ] = function( elem, name, isXML ) {
		var ret, handle,
			lowercaseName = name.toLowerCase();

		if ( !isXML ) {

			// Avoid an infinite loop by temporarily removing this function from the getter
			handle = attrHandle[ lowercaseName ];
			attrHandle[ lowercaseName ] = ret;
			ret = getter( elem, name, isXML ) != null ?
				lowercaseName :
				null;
			attrHandle[ lowercaseName ] = handle;
		}
		return ret;
	};
} );




var rfocusable = /^(?:input|select|textarea|button)$/i,
	rclickable = /^(?:a|area)$/i;

jQuery.fn.extend( {
	prop: function( name, value ) {
		return access( this, jQuery.prop, name, value, arguments.length > 1 );
	},

	removeProp: function( name ) {
		return this.each( function() {
			delete this[ jQuery.propFix[ name ] || name ];
		} );
	}
} );

jQuery.extend( {
	prop: function( elem, name, value ) {
		var ret, hooks,
			nType = elem.nodeType;

		// Don't get/set properties on text, comment and attribute nodes
		if ( nType === 3 || nType === 8 || nType === 2 ) {
			return;
		}

		if ( nType !== 1 || !jQuery.isXMLDoc( elem ) ) {

			// Fix name and attach hooks
			name = jQuery.propFix[ name ] || name;
			hooks = jQuery.propHooks[ name ];
		}

		if ( value !== undefined ) {
			if ( hooks && "set" in hooks &&
				( ret = hooks.set( elem, value, name ) ) !== undefined ) {
				return ret;
			}

			return ( elem[ name ] = value );
		}

		if ( hooks && "get" in hooks && ( ret = hooks.get( elem, name ) ) !== null ) {
			return ret;
		}

		return elem[ name ];
	},

	propHooks: {
		tabIndex: {
			get: function( elem ) {

				// Support: IE <=9 - 11 only
				// elem.tabIndex doesn't always return the
				// correct value when it hasn't been explicitly set
				// https://web.archive.org/web/20141116233347/http://fluidproject.org/blog/2008/01/09/getting-setting-and-removing-tabindex-values-with-javascript/
				// Use proper attribute retrieval(#12072)
				var tabindex = jQuery.find.attr( elem, "tabindex" );

				if ( tabindex ) {
					return parseInt( tabindex, 10 );
				}

				if (
					rfocusable.test( elem.nodeName ) ||
					rclickable.test( elem.nodeName ) &&
					elem.href
				) {
					return 0;
				}

				return -1;
			}
		}
	},

	propFix: {
		"for": "htmlFor",
		"class": "className"
	}
} );

// Support: IE <=11 only
// Accessing the selectedIndex property
// forces the browser to respect setting selected
// on the option
// The getter ensures a default option is selected
// when in an optgroup
// eslint rule "no-unused-expressions" is disabled for this code
// since it considers such accessions noop
if ( !support.optSelected ) {
	jQuery.propHooks.selected = {
		get: function( elem ) {

			/* eslint no-unused-expressions: "off" */

			var parent = elem.parentNode;
			if ( parent && parent.parentNode ) {
				parent.parentNode.selectedIndex;
			}
			return null;
		},
		set: function( elem ) {

			/* eslint no-unused-expressions: "off" */

			var parent = elem.parentNode;
			if ( parent ) {
				parent.selectedIndex;

				if ( parent.parentNode ) {
					parent.parentNode.selectedIndex;
				}
			}
		}
	};
}

jQuery.each( [
	"tabIndex",
	"readOnly",
	"maxLength",
	"cellSpacing",
	"cellPadding",
	"rowSpan",
	"colSpan",
	"useMap",
	"frameBorder",
	"contentEditable"
], function() {
	jQuery.propFix[ this.toLowerCase() ] = this;
} );




	// Strip and collapse whitespace according to HTML spec
	// https://infra.spec.whatwg.org/#strip-and-collapse-ascii-whitespace
	function stripAndCollapse( value ) {
		var tokens = value.match( rnothtmlwhite ) || [];
		return tokens.join( " " );
	}


function getClass( elem ) {
	return elem.getAttribute && elem.getAttribute( "class" ) || "";
}

function classesToArray( value ) {
	if ( Array.isArray( value ) ) {
		return value;
	}
	if ( typeof value === "string" ) {
		return value.match( rnothtmlwhite ) || [];
	}
	return [];
}

jQuery.fn.extend( {
	addClass: function( value ) {
		var classes, elem, cur, curValue, clazz, j, finalValue,
			i = 0;

		if ( isFunction( value ) ) {
			return this.each( function( j ) {
				jQuery( this ).addClass( value.call( this, j, getClass( this ) ) );
			} );
		}

		classes = classesToArray( value );

		if ( classes.length ) {
			while ( ( elem = this[ i++ ] ) ) {
				curValue = getClass( elem );
				cur = elem.nodeType === 1 && ( " " + stripAndCollapse( curValue ) + " " );

				if ( cur ) {
					j = 0;
					while ( ( clazz = classes[ j++ ] ) ) {
						if ( cur.indexOf( " " + clazz + " " ) < 0 ) {
							cur += clazz + " ";
						}
					}

					// Only assign if different to avoid unneeded rendering.
					finalValue = stripAndCollapse( cur );
					if ( curValue !== finalValue ) {
						elem.setAttribute( "class", finalValue );
					}
				}
			}
		}

		return this;
	},

	removeClass: function( value ) {
		var classes, elem, cur, curValue, clazz, j, finalValue,
			i = 0;

		if ( isFunction( value ) ) {
			return this.each( function( j ) {
				jQuery( this ).removeClass( value.call( this, j, getClass( this ) ) );
			} );
		}

		if ( !arguments.length ) {
			return this.attr( "class", "" );
		}

		classes = classesToArray( value );

		if ( classes.length ) {
			while ( ( elem = this[ i++ ] ) ) {
				curValue = getClass( elem );

				// This expression is here for better compressibility (see addClass)
				cur = elem.nodeType === 1 && ( " " + stripAndCollapse( curValue ) + " " );

				if ( cur ) {
					j = 0;
					while ( ( clazz = classes[ j++ ] ) ) {

						// Remove *all* instances
						while ( cur.indexOf( " " + clazz + " " ) > -1 ) {
							cur = cur.replace( " " + clazz + " ", " " );
						}
					}

					// Only assign if different to avoid unneeded rendering.
					finalValue = stripAndCollapse( cur );
					if ( curValue !== finalValue ) {
						elem.setAttribute( "class", finalValue );
					}
				}
			}
		}

		return this;
	},

	toggleClass: function( value, stateVal ) {
		var type = typeof value,
			isValidValue = type === "string" || Array.isArray( value );

		if ( typeof stateVal === "boolean" && isValidValue ) {
			return stateVal ? this.addClass( value ) : this.removeClass( value );
		}

		if ( isFunction( value ) ) {
			return this.each( function( i ) {
				jQuery( this ).toggleClass(
					value.call( this, i, getClass( this ), stateVal ),
					stateVal
				);
			} );
		}

		return this.each( function() {
			var className, i, self, classNames;

			if ( isValidValue ) {

				// Toggle individual class names
				i = 0;
				self = jQuery( this );
				classNames = classesToArray( value );

				while ( ( className = classNames[ i++ ] ) ) {

					// Check each className given, space separated list
					if ( self.hasClass( className ) ) {
						self.removeClass( className );
					} else {
						self.addClass( className );
					}
				}

			// Toggle whole class name
			} else if ( value === undefined || type === "boolean" ) {
				className = getClass( this );
				if ( className ) {

					// Store className if set
					dataPriv.set( this, "__className__", className );
				}

				// If the element has a class name or if we're passed `false`,
				// then remove the whole classname (if there was one, the above saved it).
				// Otherwise bring back whatever was previously saved (if anything),
				// falling back to the empty string if nothing was stored.
				if ( this.setAttribute ) {
					this.setAttribute( "class",
						className || value === false ?
							"" :
							dataPriv.get( this, "__className__" ) || ""
					);
				}
			}
		} );
	},

	hasClass: function( selector ) {
		var className, elem,
			i = 0;

		className = " " + selector + " ";
		while ( ( elem = this[ i++ ] ) ) {
			if ( elem.nodeType === 1 &&
				( " " + stripAndCollapse( getClass( elem ) ) + " " ).indexOf( className ) > -1 ) {
				return true;
			}
		}

		return false;
	}
} );




var rreturn = /\r/g;

jQuery.fn.extend( {
	val: function( value ) {
		var hooks, ret, valueIsFunction,
			elem = this[ 0 ];

		if ( !arguments.length ) {
			if ( elem ) {
				hooks = jQuery.valHooks[ elem.type ] ||
					jQuery.valHooks[ elem.nodeName.toLowerCase() ];

				if ( hooks &&
					"get" in hooks &&
					( ret = hooks.get( elem, "value" ) ) !== undefined
				) {
					return ret;
				}

				ret = elem.value;

				// Handle most common string cases
				if ( typeof ret === "string" ) {
					return ret.replace( rreturn, "" );
				}

				// Handle cases where value is null/undef or number
				return ret == null ? "" : ret;
			}

			return;
		}

		valueIsFunction = isFunction( value );

		return this.each( function( i ) {
			var val;

			if ( this.nodeType !== 1 ) {
				return;
			}

			if ( valueIsFunction ) {
				val = value.call( this, i, jQuery( this ).val() );
			} else {
				val = value;
			}

			// Treat null/undefined as ""; convert numbers to string
			if ( val == null ) {
				val = "";

			} else if ( typeof val === "number" ) {
				val += "";

			} else if ( Array.isArray( val ) ) {
				val = jQuery.map( val, function( value ) {
					return value == null ? "" : value + "";
				} );
			}

			hooks = jQuery.valHooks[ this.type ] || jQuery.valHooks[ this.nodeName.toLowerCase() ];

			// If set returns undefined, fall back to normal setting
			if ( !hooks || !( "set" in hooks ) || hooks.set( this, val, "value" ) === undefined ) {
				this.value = val;
			}
		} );
	}
} );

jQuery.extend( {
	valHooks: {
		option: {
			get: function( elem ) {

				var val = jQuery.find.attr( elem, "value" );
				return val != null ?
					val :

					// Support: IE <=10 - 11 only
					// option.text throws exceptions (#14686, #14858)
					// Strip and collapse whitespace
					// https://html.spec.whatwg.org/#strip-and-collapse-whitespace
					stripAndCollapse( jQuery.text( elem ) );
			}
		},
		select: {
			get: function( elem ) {
				var value, option, i,
					options = elem.options,
					index = elem.selectedIndex,
					one = elem.type === "select-one",
					values = one ? null : [],
					max = one ? index + 1 : options.length;

				if ( index < 0 ) {
					i = max;

				} else {
					i = one ? index : 0;
				}

				// Loop through all the selected options
				for ( ; i < max; i++ ) {
					option = options[ i ];

					// Support: IE <=9 only
					// IE8-9 doesn't update selected after form reset (#2551)
					if ( ( option.selected || i === index ) &&

							// Don't return options that are disabled or in a disabled optgroup
							!option.disabled &&
							( !option.parentNode.disabled ||
								!nodeName( option.parentNode, "optgroup" ) ) ) {

						// Get the specific value for the option
						value = jQuery( option ).val();

						// We don't need an array for one selects
						if ( one ) {
							return value;
						}

						// Multi-Selects return an array
						values.push( value );
					}
				}

				return values;
			},

			set: function( elem, value ) {
				var optionSet, option,
					options = elem.options,
					values = jQuery.makeArray( value ),
					i = options.length;

				while ( i-- ) {
					option = options[ i ];

					/* eslint-disable no-cond-assign */

					if ( option.selected =
						jQuery.inArray( jQuery.valHooks.option.get( option ), values ) > -1
					) {
						optionSet = true;
					}

					/* eslint-enable no-cond-assign */
				}

				// Force browsers to behave consistently when non-matching value is set
				if ( !optionSet ) {
					elem.selectedIndex = -1;
				}
				return values;
			}
		}
	}
} );

// Radios and checkboxes getter/setter
jQuery.each( [ "radio", "checkbox" ], function() {
	jQuery.valHooks[ this ] = {
		set: function( elem, value ) {
			if ( Array.isArray( value ) ) {
				return ( elem.checked = jQuery.inArray( jQuery( elem ).val(), value ) > -1 );
			}
		}
	};
	if ( !support.checkOn ) {
		jQuery.valHooks[ this ].get = function( elem ) {
			return elem.getAttribute( "value" ) === null ? "on" : elem.value;
		};
	}
} );




// Return jQuery for attributes-only inclusion


support.focusin = "onfocusin" in window;


var rfocusMorph = /^(?:focusinfocus|focusoutblur)$/,
	stopPropagationCallback = function( e ) {
		e.stopPropagation();
	};

jQuery.extend( jQuery.event, {

	trigger: function( event, data, elem, onlyHandlers ) {

		var i, cur, tmp, bubbleType, ontype, handle, special, lastElement,
			eventPath = [ elem || document ],
			type = hasOwn.call( event, "type" ) ? event.type : event,
			namespaces = hasOwn.call( event, "namespace" ) ? event.namespace.split( "." ) : [];

		cur = lastElement = tmp = elem = elem || document;

		// Don't do events on text and comment nodes
		if ( elem.nodeType === 3 || elem.nodeType === 8 ) {
			return;
		}

		// focus/blur morphs to focusin/out; ensure we're not firing them right now
		if ( rfocusMorph.test( type + jQuery.event.triggered ) ) {
			return;
		}

		if ( type.indexOf( "." ) > -1 ) {

			// Namespaced trigger; create a regexp to match event type in handle()
			namespaces = type.split( "." );
			type = namespaces.shift();
			namespaces.sort();
		}
		ontype = type.indexOf( ":" ) < 0 && "on" + type;

		// Caller can pass in a jQuery.Event object, Object, or just an event type string
		event = event[ jQuery.expando ] ?
			event :
			new jQuery.Event( type, typeof event === "object" && event );

		// Trigger bitmask: & 1 for native handlers; & 2 for jQuery (always true)
		event.isTrigger = onlyHandlers ? 2 : 3;
		event.namespace = namespaces.join( "." );
		event.rnamespace = event.namespace ?
			new RegExp( "(^|\\.)" + namespaces.join( "\\.(?:.*\\.|)" ) + "(\\.|$)" ) :
			null;

		// Clean up the event in case it is being reused
		event.result = undefined;
		if ( !event.target ) {
			event.target = elem;
		}

		// Clone any incoming data and prepend the event, creating the handler arg list
		data = data == null ?
			[ event ] :
			jQuery.makeArray( data, [ event ] );

		// Allow special events to draw outside the lines
		special = jQuery.event.special[ type ] || {};
		if ( !onlyHandlers && special.trigger && special.trigger.apply( elem, data ) === false ) {
			return;
		}

		// Determine event propagation path in advance, per W3C events spec (#9951)
		// Bubble up to document, then to window; watch for a global ownerDocument var (#9724)
		if ( !onlyHandlers && !special.noBubble && !isWindow( elem ) ) {

			bubbleType = special.delegateType || type;
			if ( !rfocusMorph.test( bubbleType + type ) ) {
				cur = cur.parentNode;
			}
			for ( ; cur; cur = cur.parentNode ) {
				eventPath.push( cur );
				tmp = cur;
			}

			// Only add window if we got to document (e.g., not plain obj or detached DOM)
			if ( tmp === ( elem.ownerDocument || document ) ) {
				eventPath.push( tmp.defaultView || tmp.parentWindow || window );
			}
		}

		// Fire handlers on the event path
		i = 0;
		while ( ( cur = eventPath[ i++ ] ) && !event.isPropagationStopped() ) {
			lastElement = cur;
			event.type = i > 1 ?
				bubbleType :
				special.bindType || type;

			// jQuery handler
			handle = ( dataPriv.get( cur, "events" ) || Object.create( null ) )[ event.type ] &&
				dataPriv.get( cur, "handle" );
			if ( handle ) {
				handle.apply( cur, data );
			}

			// Native handler
			handle = ontype && cur[ ontype ];
			if ( handle && handle.apply && acceptData( cur ) ) {
				event.result = handle.apply( cur, data );
				if ( event.result === false ) {
					event.preventDefault();
				}
			}
		}
		event.type = type;

		// If nobody prevented the default action, do it now
		if ( !onlyHandlers && !event.isDefaultPrevented() ) {

			if ( ( !special._default ||
				special._default.apply( eventPath.pop(), data ) === false ) &&
				acceptData( elem ) ) {

				// Call a native DOM method on the target with the same name as the event.
				// Don't do default actions on window, that's where global variables be (#6170)
				if ( ontype && isFunction( elem[ type ] ) && !isWindow( elem ) ) {

					// Don't re-trigger an onFOO event when we call its FOO() method
					tmp = elem[ ontype ];

					if ( tmp ) {
						elem[ ontype ] = null;
					}

					// Prevent re-triggering of the same event, since we already bubbled it above
					jQuery.event.triggered = type;

					if ( event.isPropagationStopped() ) {
						lastElement.addEventListener( type, stopPropagationCallback );
					}

					elem[ type ]();

					if ( event.isPropagationStopped() ) {
						lastElement.removeEventListener( type, stopPropagationCallback );
					}

					jQuery.event.triggered = undefined;

					if ( tmp ) {
						elem[ ontype ] = tmp;
					}
				}
			}
		}

		return event.result;
	},

	// Piggyback on a donor event to simulate a different one
	// Used only for `focus(in | out)` events
	simulate: function( type, elem, event ) {
		var e = jQuery.extend(
			new jQuery.Event(),
			event,
			{
				type: type,
				isSimulated: true
			}
		);

		jQuery.event.trigger( e, null, elem );
	}

} );

jQuery.fn.extend( {

	trigger: function( type, data ) {
		return this.each( function() {
			jQuery.event.trigger( type, data, this );
		} );
	},
	triggerHandler: function( type, data ) {
		var elem = this[ 0 ];
		if ( elem ) {
			return jQuery.event.trigger( type, data, elem, true );
		}
	}
} );


// Support: Firefox <=44
// Firefox doesn't have focus(in | out) events
// Related ticket - https://bugzilla.mozilla.org/show_bug.cgi?id=687787
//
// Support: Chrome <=48 - 49, Safari <=9.0 - 9.1
// focus(in | out) events fire after focus & blur events,
// which is spec violation - http://www.w3.org/TR/DOM-Level-3-Events/#events-focusevent-event-order
// Related ticket - https://bugs.chromium.org/p/chromium/issues/detail?id=449857
if ( !support.focusin ) {
	jQuery.each( { focus: "focusin", blur: "focusout" }, function( orig, fix ) {

		// Attach a single capturing handler on the document while someone wants focusin/focusout
		var handler = function( event ) {
			jQuery.event.simulate( fix, event.target, jQuery.event.fix( event ) );
		};

		jQuery.event.special[ fix ] = {
			setup: function() {

				// Handle: regular nodes (via `this.ownerDocument`), window
				// (via `this.document`) & document (via `this`).
				var doc = this.ownerDocument || this.document || this,
					attaches = dataPriv.access( doc, fix );

				if ( !attaches ) {
					doc.addEventListener( orig, handler, true );
				}
				dataPriv.access( doc, fix, ( attaches || 0 ) + 1 );
			},
			teardown: function() {
				var doc = this.ownerDocument || this.document || this,
					attaches = dataPriv.access( doc, fix ) - 1;

				if ( !attaches ) {
					doc.removeEventListener( orig, handler, true );
					dataPriv.remove( doc, fix );

				} else {
					dataPriv.access( doc, fix, attaches );
				}
			}
		};
	} );
}
var location = window.location;

var nonce = { guid: Date.now() };

var rquery = ( /\?/ );



// Cross-browser xml parsing
jQuery.parseXML = function( data ) {
	var xml, parserErrorElem;
	if ( !data || typeof data !== "string" ) {
		return null;
	}

	// Support: IE 9 - 11 only
	// IE throws on parseFromString with invalid input.
	try {
		xml = ( new window.DOMParser() ).parseFromString( data, "text/xml" );
	} catch ( e ) {}

	parserErrorElem = xml && xml.getElementsByTagName( "parsererror" )[ 0 ];
	if ( !xml || parserErrorElem ) {
		jQuery.error( "Invalid XML: " + (
			parserErrorElem ?
				jQuery.map( parserErrorElem.childNodes, function( el ) {
					return el.textContent;
				} ).join( "\n" ) :
				data
		) );
	}
	return xml;
};


var
	rbracket = /\[\]$/,
	rCRLF = /\r?\n/g,
	rsubmitterTypes = /^(?:submit|button|image|reset|file)$/i,
	rsubmittable = /^(?:input|select|textarea|keygen)/i;

function buildParams( prefix, obj, traditional, add ) {
	var name;

	if ( Array.isArray( obj ) ) {

		// Serialize array item.
		jQuery.each( obj, function( i, v ) {
			if ( traditional || rbracket.test( prefix ) ) {

				// Treat each array item as a scalar.
				add( prefix, v );

			} else {

				// Item is non-scalar (array or object), encode its numeric index.
				buildParams(
					prefix + "[" + ( typeof v === "object" && v != null ? i : "" ) + "]",
					v,
					traditional,
					add
				);
			}
		} );

	} else if ( !traditional && toType( obj ) === "object" ) {

		// Serialize object item.
		for ( name in obj ) {
			buildParams( prefix + "[" + name + "]", obj[ name ], traditional, add );
		}

	} else {

		// Serialize scalar item.
		add( prefix, obj );
	}
}

// Serialize an array of form elements or a set of
// key/values into a query string
jQuery.param = function( a, traditional ) {
	var prefix,
		s = [],
		add = function( key, valueOrFunction ) {

			// If value is a function, invoke it and use its return value
			var value = isFunction( valueOrFunction ) ?
				valueOrFunction() :
				valueOrFunction;

			s[ s.length ] = encodeURIComponent( key ) + "=" +
				encodeURIComponent( value == null ? "" : value );
		};

	if ( a == null ) {
		return "";
	}

	// If an array was passed in, assume that it is an array of form elements.
	if ( Array.isArray( a ) || ( a.jquery && !jQuery.isPlainObject( a ) ) ) {

		// Serialize the form elements
		jQuery.each( a, function() {
			add( this.name, this.value );
		} );

	} else {

		// If traditional, encode the "old" way (the way 1.3.2 or older
		// did it), otherwise encode params recursively.
		for ( prefix in a ) {
			buildParams( prefix, a[ prefix ], traditional, add );
		}
	}

	// Return the resulting serialization
	return s.join( "&" );
};

jQuery.fn.extend( {
	serialize: function() {
		return jQuery.param( this.serializeArray() );
	},
	serializeArray: function() {
		return this.map( function() {

			// Can add propHook for "elements" to filter or add form elements
			var elements = jQuery.prop( this, "elements" );
			return elements ? jQuery.makeArray( elements ) : this;
		} ).filter( function() {
			var type = this.type;

			// Use .is( ":disabled" ) so that fieldset[disabled] works
			return this.name && !jQuery( this ).is( ":disabled" ) &&
				rsubmittable.test( this.nodeName ) && !rsubmitterTypes.test( type ) &&
				( this.checked || !rcheckableType.test( type ) );
		} ).map( function( _i, elem ) {
			var val = jQuery( this ).val();

			if ( val == null ) {
				return null;
			}

			if ( Array.isArray( val ) ) {
				return jQuery.map( val, function( val ) {
					return { name: elem.name, value: val.replace( rCRLF, "\r\n" ) };
				} );
			}

			return { name: elem.name, value: val.replace( rCRLF, "\r\n" ) };
		} ).get();
	}
} );


var
	r20 = /%20/g,
	rhash = /#.*$/,
	rantiCache = /([?&])_=[^&]*/,
	rheaders = /^(.*?):[ \t]*([^\r\n]*)$/mg,

	// #7653, #8125, #8152: local protocol detection
	rlocalProtocol = /^(?:about|app|app-storage|.+-extension|file|res|widget):$/,
	rnoContent = /^(?:GET|HEAD)$/,
	rprotocol = /^\/\//,

	/* Prefilters
	 * 1) They are useful to introduce custom dataTypes (see ajax/jsonp.js for an example)
	 * 2) These are called:
	 *    - BEFORE asking for a transport
	 *    - AFTER param serialization (s.data is a string if s.processData is true)
	 * 3) key is the dataType
	 * 4) the catchall symbol "*" can be used
	 * 5) execution will start with transport dataType and THEN continue down to "*" if needed
	 */
	prefilters = {},

	/* Transports bindings
	 * 1) key is the dataType
	 * 2) the catchall symbol "*" can be used
	 * 3) selection will start with transport dataType and THEN go to "*" if needed
	 */
	transports = {},

	// Avoid comment-prolog char sequence (#10098); must appease lint and evade compression
	allTypes = "*/".concat( "*" ),

	// Anchor tag for parsing the document origin
	originAnchor = document.createElement( "a" );

originAnchor.href = location.href;

// Base "constructor" for jQuery.ajaxPrefilter and jQuery.ajaxTransport
function addToPrefiltersOrTransports( structure ) {

	// dataTypeExpression is optional and defaults to "*"
	return function( dataTypeExpression, func ) {

		if ( typeof dataTypeExpression !== "string" ) {
			func = dataTypeExpression;
			dataTypeExpression = "*";
		}

		var dataType,
			i = 0,
			dataTypes = dataTypeExpression.toLowerCase().match( rnothtmlwhite ) || [];

		if ( isFunction( func ) ) {

			// For each dataType in the dataTypeExpression
			while ( ( dataType = dataTypes[ i++ ] ) ) {

				// Prepend if requested
				if ( dataType[ 0 ] === "+" ) {
					dataType = dataType.slice( 1 ) || "*";
					( structure[ dataType ] = structure[ dataType ] || [] ).unshift( func );

				// Otherwise append
				} else {
					( structure[ dataType ] = structure[ dataType ] || [] ).push( func );
				}
			}
		}
	};
}

// Base inspection function for prefilters and transports
function inspectPrefiltersOrTransports( structure, options, originalOptions, jqXHR ) {

	var inspected = {},
		seekingTransport = ( structure === transports );

	function inspect( dataType ) {
		var selected;
		inspected[ dataType ] = true;
		jQuery.each( structure[ dataType ] || [], function( _, prefilterOrFactory ) {
			var dataTypeOrTransport = prefilterOrFactory( options, originalOptions, jqXHR );
			if ( typeof dataTypeOrTransport === "string" &&
				!seekingTransport && !inspected[ dataTypeOrTransport ] ) {

				options.dataTypes.unshift( dataTypeOrTransport );
				inspect( dataTypeOrTransport );
				return false;
			} else if ( seekingTransport ) {
				return !( selected = dataTypeOrTransport );
			}
		} );
		return selected;
	}

	return inspect( options.dataTypes[ 0 ] ) || !inspected[ "*" ] && inspect( "*" );
}

// A special extend for ajax options
// that takes "flat" options (not to be deep extended)
// Fixes #9887
function ajaxExtend( target, src ) {
	var key, deep,
		flatOptions = jQuery.ajaxSettings.flatOptions || {};

	for ( key in src ) {
		if ( src[ key ] !== undefined ) {
			( flatOptions[ key ] ? target : ( deep || ( deep = {} ) ) )[ key ] = src[ key ];
		}
	}
	if ( deep ) {
		jQuery.extend( true, target, deep );
	}

	return target;
}

/* Handles responses to an ajax request:
 * - finds the right dataType (mediates between content-type and expected dataType)
 * - returns the corresponding response
 */
function ajaxHandleResponses( s, jqXHR, responses ) {

	var ct, type, finalDataType, firstDataType,
		contents = s.contents,
		dataTypes = s.dataTypes;

	// Remove auto dataType and get content-type in the process
	while ( dataTypes[ 0 ] === "*" ) {
		dataTypes.shift();
		if ( ct === undefined ) {
			ct = s.mimeType || jqXHR.getResponseHeader( "Content-Type" );
		}
	}

	// Check if we're dealing with a known content-type
	if ( ct ) {
		for ( type in contents ) {
			if ( contents[ type ] && contents[ type ].test( ct ) ) {
				dataTypes.unshift( type );
				break;
			}
		}
	}

	// Check to see if we have a response for the expected dataType
	if ( dataTypes[ 0 ] in responses ) {
		finalDataType = dataTypes[ 0 ];
	} else {

		// Try convertible dataTypes
		for ( type in responses ) {
			if ( !dataTypes[ 0 ] || s.converters[ type + " " + dataTypes[ 0 ] ] ) {
				finalDataType = type;
				break;
			}
			if ( !firstDataType ) {
				firstDataType = type;
			}
		}

		// Or just use first one
		finalDataType = finalDataType || firstDataType;
	}

	// If we found a dataType
	// We add the dataType to the list if needed
	// and return the corresponding response
	if ( finalDataType ) {
		if ( finalDataType !== dataTypes[ 0 ] ) {
			dataTypes.unshift( finalDataType );
		}
		return responses[ finalDataType ];
	}
}

/* Chain conversions given the request and the original response
 * Also sets the responseXXX fields on the jqXHR instance
 */
function ajaxConvert( s, response, jqXHR, isSuccess ) {
	var conv2, current, conv, tmp, prev,
		converters = {},

		// Work with a copy of dataTypes in case we need to modify it for conversion
		dataTypes = s.dataTypes.slice();

	// Create converters map with lowercased keys
	if ( dataTypes[ 1 ] ) {
		for ( conv in s.converters ) {
			converters[ conv.toLowerCase() ] = s.converters[ conv ];
		}
	}

	current = dataTypes.shift();

	// Convert to each sequential dataType
	while ( current ) {

		if ( s.responseFields[ current ] ) {
			jqXHR[ s.responseFields[ current ] ] = response;
		}

		// Apply the dataFilter if provided
		if ( !prev && isSuccess && s.dataFilter ) {
			response = s.dataFilter( response, s.dataType );
		}

		prev = current;
		current = dataTypes.shift();

		if ( current ) {

			// There's only work to do if current dataType is non-auto
			if ( current === "*" ) {

				current = prev;

			// Convert response if prev dataType is non-auto and differs from current
			} else if ( prev !== "*" && prev !== current ) {

				// Seek a direct converter
				conv = converters[ prev + " " + current ] || converters[ "* " + current ];

				// If none found, seek a pair
				if ( !conv ) {
					for ( conv2 in converters ) {

						// If conv2 outputs current
						tmp = conv2.split( " " );
						if ( tmp[ 1 ] === current ) {

							// If prev can be converted to accepted input
							conv = converters[ prev + " " + tmp[ 0 ] ] ||
								converters[ "* " + tmp[ 0 ] ];
							if ( conv ) {

								// Condense equivalence converters
								if ( conv === true ) {
									conv = converters[ conv2 ];

								// Otherwise, insert the intermediate dataType
								} else if ( converters[ conv2 ] !== true ) {
									current = tmp[ 0 ];
									dataTypes.unshift( tmp[ 1 ] );
								}
								break;
							}
						}
					}
				}

				// Apply converter (if not an equivalence)
				if ( conv !== true ) {

					// Unless errors are allowed to bubble, catch and return them
					if ( conv && s.throws ) {
						response = conv( response );
					} else {
						try {
							response = conv( response );
						} catch ( e ) {
							return {
								state: "parsererror",
								error: conv ? e : "No conversion from " + prev + " to " + current
							};
						}
					}
				}
			}
		}
	}

	return { state: "success", data: response };
}

jQuery.extend( {

	// Counter for holding the number of active queries
	active: 0,

	// Last-Modified header cache for next request
	lastModified: {},
	etag: {},

	ajaxSettings: {
		url: location.href,
		type: "GET",
		isLocal: rlocalProtocol.test( location.protocol ),
		global: true,
		processData: true,
		async: true,
		contentType: "application/x-www-form-urlencoded; charset=UTF-8",

		/*
		timeout: 0,
		data: null,
		dataType: null,
		username: null,
		password: null,
		cache: null,
		throws: false,
		traditional: false,
		headers: {},
		*/

		accepts: {
			"*": allTypes,
			text: "text/plain",
			html: "text/html",
			xml: "application/xml, text/xml",
			json: "application/json, text/javascript"
		},

		contents: {
			xml: /\bxml\b/,
			html: /\bhtml/,
			json: /\bjson\b/
		},

		responseFields: {
			xml: "responseXML",
			text: "responseText",
			json: "responseJSON"
		},

		// Data converters
		// Keys separate source (or catchall "*") and destination types with a single space
		converters: {

			// Convert anything to text
			"* text": String,

			// Text to html (true = no transformation)
			"text html": true,

			// Evaluate text as a json expression
			"text json": JSON.parse,

			// Parse text as xml
			"text xml": jQuery.parseXML
		},

		// For options that shouldn't be deep extended:
		// you can add your own custom options here if
		// and when you create one that shouldn't be
		// deep extended (see ajaxExtend)
		flatOptions: {
			url: true,
			context: true
		}
	},

	// Creates a full fledged settings object into target
	// with both ajaxSettings and settings fields.
	// If target is omitted, writes into ajaxSettings.
	ajaxSetup: function( target, settings ) {
		return settings ?

			// Building a settings object
			ajaxExtend( ajaxExtend( target, jQuery.ajaxSettings ), settings ) :

			// Extending ajaxSettings
			ajaxExtend( jQuery.ajaxSettings, target );
	},

	ajaxPrefilter: addToPrefiltersOrTransports( prefilters ),
	ajaxTransport: addToPrefiltersOrTransports( transports ),

	// Main method
	ajax: function( url, options ) {

		// If url is an object, simulate pre-1.5 signature
		if ( typeof url === "object" ) {
			options = url;
			url = undefined;
		}

		// Force options to be an object
		options = options || {};

		var transport,

			// URL without anti-cache param
			cacheURL,

			// Response headers
			responseHeadersString,
			responseHeaders,

			// timeout handle
			timeoutTimer,

			// Url cleanup var
			urlAnchor,

			// Request state (becomes false upon send and true upon completion)
			completed,

			// To know if global events are to be dispatched
			fireGlobals,

			// Loop variable
			i,

			// uncached part of the url
			uncached,

			// Create the final options object
			s = jQuery.ajaxSetup( {}, options ),

			// Callbacks context
			callbackContext = s.context || s,

			// Context for global events is callbackContext if it is a DOM node or jQuery collection
			globalEventContext = s.context &&
				( callbackContext.nodeType || callbackContext.jquery ) ?
				jQuery( callbackContext ) :
				jQuery.event,

			// Deferreds
			deferred = jQuery.Deferred(),
			completeDeferred = jQuery.Callbacks( "once memory" ),

			// Status-dependent callbacks
			statusCode = s.statusCode || {},

			// Headers (they are sent all at once)
			requestHeaders = {},
			requestHeadersNames = {},

			// Default abort message
			strAbort = "canceled",

			// Fake xhr
			jqXHR = {
				readyState: 0,

				// Builds headers hashtable if needed
				getResponseHeader: function( key ) {
					var match;
					if ( completed ) {
						if ( !responseHeaders ) {
							responseHeaders = {};
							while ( ( match = rheaders.exec( responseHeadersString ) ) ) {
								responseHeaders[ match[ 1 ].toLowerCase() + " " ] =
									( responseHeaders[ match[ 1 ].toLowerCase() + " " ] || [] )
										.concat( match[ 2 ] );
							}
						}
						match = responseHeaders[ key.toLowerCase() + " " ];
					}
					return match == null ? null : match.join( ", " );
				},

				// Raw string
				getAllResponseHeaders: function() {
					return completed ? responseHeadersString : null;
				},

				// Caches the header
				setRequestHeader: function( name, value ) {
					if ( completed == null ) {
						name = requestHeadersNames[ name.toLowerCase() ] =
							requestHeadersNames[ name.toLowerCase() ] || name;
						requestHeaders[ name ] = value;
					}
					return this;
				},

				// Overrides response content-type header
				overrideMimeType: function( type ) {
					if ( completed == null ) {
						s.mimeType = type;
					}
					return this;
				},

				// Status-dependent callbacks
				statusCode: function( map ) {
					var code;
					if ( map ) {
						if ( completed ) {

							// Execute the appropriate callbacks
							jqXHR.always( map[ jqXHR.status ] );
						} else {

							// Lazy-add the new callbacks in a way that preserves old ones
							for ( code in map ) {
								statusCode[ code ] = [ statusCode[ code ], map[ code ] ];
							}
						}
					}
					return this;
				},

				// Cancel the request
				abort: function( statusText ) {
					var finalText = statusText || strAbort;
					if ( transport ) {
						transport.abort( finalText );
					}
					done( 0, finalText );
					return this;
				}
			};

		// Attach deferreds
		deferred.promise( jqXHR );

		// Add protocol if not provided (prefilters might expect it)
		// Handle falsy url in the settings object (#10093: consistency with old signature)
		// We also use the url parameter if available
		s.url = ( ( url || s.url || location.href ) + "" )
			.replace( rprotocol, location.protocol + "//" );

		// Alias method option to type as per ticket #12004
		s.type = options.method || options.type || s.method || s.type;

		// Extract dataTypes list
		s.dataTypes = ( s.dataType || "*" ).toLowerCase().match( rnothtmlwhite ) || [ "" ];

		// A cross-domain request is in order when the origin doesn't match the current origin.
		if ( s.crossDomain == null ) {
			urlAnchor = document.createElement( "a" );

			// Support: IE <=8 - 11, Edge 12 - 15
			// IE throws exception on accessing the href property if url is malformed,
			// e.g. http://example.com:80x/
			try {
				urlAnchor.href = s.url;

				// Support: IE <=8 - 11 only
				// Anchor's host property isn't correctly set when s.url is relative
				urlAnchor.href = urlAnchor.href;
				s.crossDomain = originAnchor.protocol + "//" + originAnchor.host !==
					urlAnchor.protocol + "//" + urlAnchor.host;
			} catch ( e ) {

				// If there is an error parsing the URL, assume it is crossDomain,
				// it can be rejected by the transport if it is invalid
				s.crossDomain = true;
			}
		}

		// Convert data if not already a string
		if ( s.data && s.processData && typeof s.data !== "string" ) {
			s.data = jQuery.param( s.data, s.traditional );
		}

		// Apply prefilters
		inspectPrefiltersOrTransports( prefilters, s, options, jqXHR );

		// If request was aborted inside a prefilter, stop there
		if ( completed ) {
			return jqXHR;
		}

		// We can fire global events as of now if asked to
		// Don't fire events if jQuery.event is undefined in an AMD-usage scenario (#15118)
		fireGlobals = jQuery.event && s.global;

		// Watch for a new set of requests
		if ( fireGlobals && jQuery.active++ === 0 ) {
			jQuery.event.trigger( "ajaxStart" );
		}

		// Uppercase the type
		s.type = s.type.toUpperCase();

		// Determine if request has content
		s.hasContent = !rnoContent.test( s.type );

		// Save the URL in case we're toying with the If-Modified-Since
		// and/or If-None-Match header later on
		// Remove hash to simplify url manipulation
		cacheURL = s.url.replace( rhash, "" );

		// More options handling for requests with no content
		if ( !s.hasContent ) {

			// Remember the hash so we can put it back
			uncached = s.url.slice( cacheURL.length );

			// If data is available and should be processed, append data to url
			if ( s.data && ( s.processData || typeof s.data === "string" ) ) {
				cacheURL += ( rquery.test( cacheURL ) ? "&" : "?" ) + s.data;

				// #9682: remove data so that it's not used in an eventual retry
				delete s.data;
			}

			// Add or update anti-cache param if needed
			if ( s.cache === false ) {
				cacheURL = cacheURL.replace( rantiCache, "$1" );
				uncached = ( rquery.test( cacheURL ) ? "&" : "?" ) + "_=" + ( nonce.guid++ ) +
					uncached;
			}

			// Put hash and anti-cache on the URL that will be requested (gh-1732)
			s.url = cacheURL + uncached;

		// Change '%20' to '+' if this is encoded form body content (gh-2658)
		} else if ( s.data && s.processData &&
			( s.contentType || "" ).indexOf( "application/x-www-form-urlencoded" ) === 0 ) {
			s.data = s.data.replace( r20, "+" );
		}

		// Set the If-Modified-Since and/or If-None-Match header, if in ifModified mode.
		if ( s.ifModified ) {
			if ( jQuery.lastModified[ cacheURL ] ) {
				jqXHR.setRequestHeader( "If-Modified-Since", jQuery.lastModified[ cacheURL ] );
			}
			if ( jQuery.etag[ cacheURL ] ) {
				jqXHR.setRequestHeader( "If-None-Match", jQuery.etag[ cacheURL ] );
			}
		}

		// Set the correct header, if data is being sent
		if ( s.data && s.hasContent && s.contentType !== false || options.contentType ) {
			jqXHR.setRequestHeader( "Content-Type", s.contentType );
		}

		// Set the Accepts header for the server, depending on the dataType
		jqXHR.setRequestHeader(
			"Accept",
			s.dataTypes[ 0 ] && s.accepts[ s.dataTypes[ 0 ] ] ?
				s.accepts[ s.dataTypes[ 0 ] ] +
					( s.dataTypes[ 0 ] !== "*" ? ", " + allTypes + "; q=0.01" : "" ) :
				s.accepts[ "*" ]
		);

		// Check for headers option
		for ( i in s.headers ) {
			jqXHR.setRequestHeader( i, s.headers[ i ] );
		}

		// Allow custom headers/mimetypes and early abort
		if ( s.beforeSend &&
			( s.beforeSend.call( callbackContext, jqXHR, s ) === false || completed ) ) {

			// Abort if not done already and return
			return jqXHR.abort();
		}

		// Aborting is no longer a cancellation
		strAbort = "abort";

		// Install callbacks on deferreds
		completeDeferred.add( s.complete );
		jqXHR.done( s.success );
		jqXHR.fail( s.error );

		// Get transport
		transport = inspectPrefiltersOrTransports( transports, s, options, jqXHR );

		// If no transport, we auto-abort
		if ( !transport ) {
			done( -1, "No Transport" );
		} else {
			jqXHR.readyState = 1;

			// Send global event
			if ( fireGlobals ) {
				globalEventContext.trigger( "ajaxSend", [ jqXHR, s ] );
			}

			// If request was aborted inside ajaxSend, stop there
			if ( completed ) {
				return jqXHR;
			}

			// Timeout
			if ( s.async && s.timeout > 0 ) {
				timeoutTimer = window.setTimeout( function() {
					jqXHR.abort( "timeout" );
				}, s.timeout );
			}

			try {
				completed = false;
				transport.send( requestHeaders, done );
			} catch ( e ) {

				// Rethrow post-completion exceptions
				if ( completed ) {
					throw e;
				}

				// Propagate others as results
				done( -1, e );
			}
		}

		// Callback for when everything is done
		function done( status, nativeStatusText, responses, headers ) {
			var isSuccess, success, error, response, modified,
				statusText = nativeStatusText;

			// Ignore repeat invocations
			if ( completed ) {
				return;
			}

			completed = true;

			// Clear timeout if it exists
			if ( timeoutTimer ) {
				window.clearTimeout( timeoutTimer );
			}

			// Dereference transport for early garbage collection
			// (no matter how long the jqXHR object will be used)
			transport = undefined;

			// Cache response headers
			responseHeadersString = headers || "";

			// Set readyState
			jqXHR.readyState = status > 0 ? 4 : 0;

			// Determine if successful
			isSuccess = status >= 200 && status < 300 || status === 304;

			// Get response data
			if ( responses ) {
				response = ajaxHandleResponses( s, jqXHR, responses );
			}

			// Use a noop converter for missing script but not if jsonp
			if ( !isSuccess &&
				jQuery.inArray( "script", s.dataTypes ) > -1 &&
				jQuery.inArray( "json", s.dataTypes ) < 0 ) {
				s.converters[ "text script" ] = function() {};
			}

			// Convert no matter what (that way responseXXX fields are always set)
			response = ajaxConvert( s, response, jqXHR, isSuccess );

			// If successful, handle type chaining
			if ( isSuccess ) {

				// Set the If-Modified-Since and/or If-None-Match header, if in ifModified mode.
				if ( s.ifModified ) {
					modified = jqXHR.getResponseHeader( "Last-Modified" );
					if ( modified ) {
						jQuery.lastModified[ cacheURL ] = modified;
					}
					modified = jqXHR.getResponseHeader( "etag" );
					if ( modified ) {
						jQuery.etag[ cacheURL ] = modified;
					}
				}

				// if no content
				if ( status === 204 || s.type === "HEAD" ) {
					statusText = "nocontent";

				// if not modified
				} else if ( status === 304 ) {
					statusText = "notmodified";

				// If we have data, let's convert it
				} else {
					statusText = response.state;
					success = response.data;
					error = response.error;
					isSuccess = !error;
				}
			} else {

				// Extract error from statusText and normalize for non-aborts
				error = statusText;
				if ( status || !statusText ) {
					statusText = "error";
					if ( status < 0 ) {
						status = 0;
					}
				}
			}

			// Set data for the fake xhr object
			jqXHR.status = status;
			jqXHR.statusText = ( nativeStatusText || statusText ) + "";

			// Success/Error
			if ( isSuccess ) {
				deferred.resolveWith( callbackContext, [ success, statusText, jqXHR ] );
			} else {
				deferred.rejectWith( callbackContext, [ jqXHR, statusText, error ] );
			}

			// Status-dependent callbacks
			jqXHR.statusCode( statusCode );
			statusCode = undefined;

			if ( fireGlobals ) {
				globalEventContext.trigger( isSuccess ? "ajaxSuccess" : "ajaxError",
					[ jqXHR, s, isSuccess ? success : error ] );
			}

			// Complete
			completeDeferred.fireWith( callbackContext, [ jqXHR, statusText ] );

			if ( fireGlobals ) {
				globalEventContext.trigger( "ajaxComplete", [ jqXHR, s ] );

				// Handle the global AJAX counter
				if ( !( --jQuery.active ) ) {
					jQuery.event.trigger( "ajaxStop" );
				}
			}
		}

		return jqXHR;
	},

	getJSON: function( url, data, callback ) {
		return jQuery.get( url, data, callback, "json" );
	},

	getScript: function( url, callback ) {
		return jQuery.get( url, undefined, callback, "script" );
	}
} );

jQuery.each( [ "get", "post" ], function( _i, method ) {
	jQuery[ method ] = function( url, data, callback, type ) {

		// Shift arguments if data argument was omitted
		if ( isFunction( data ) ) {
			type = type || callback;
			callback = data;
			data = undefined;
		}

		// The url can be an options object (which then must have .url)
		return jQuery.ajax( jQuery.extend( {
			url: url,
			type: method,
			dataType: type,
			data: data,
			success: callback
		}, jQuery.isPlainObject( url ) && url ) );
	};
} );

jQuery.ajaxPrefilter( function( s ) {
	var i;
	for ( i in s.headers ) {
		if ( i.toLowerCase() === "content-type" ) {
			s.contentType = s.headers[ i ] || "";
		}
	}
} );


jQuery._evalUrl = function( url, options, doc ) {
	return jQuery.ajax( {
		url: url,

		// Make this explicit, since user can override this through ajaxSetup (#11264)
		type: "GET",
		dataType: "script",
		cache: true,
		async: false,
		global: false,

		// Only evaluate the response if it is successful (gh-4126)
		// dataFilter is not invoked for failure responses, so using it instead
		// of the default converter is kludgy but it works.
		converters: {
			"text script": function() {}
		},
		dataFilter: function( response ) {
			jQuery.globalEval( response, options, doc );
		}
	} );
};


jQuery.fn.extend( {
	wrapAll: function( html ) {
		var wrap;

		if ( this[ 0 ] ) {
			if ( isFunction( html ) ) {
				html = html.call( this[ 0 ] );
			}

			// The elements to wrap the target around
			wrap = jQuery( html, this[ 0 ].ownerDocument ).eq( 0 ).clone( true );

			if ( this[ 0 ].parentNode ) {
				wrap.insertBefore( this[ 0 ] );
			}

			wrap.map( function() {
				var elem = this;

				while ( elem.firstElementChild ) {
					elem = elem.firstElementChild;
				}

				return elem;
			} ).append( this );
		}

		return this;
	},

	wrapInner: function( html ) {
		if ( isFunction( html ) ) {
			return this.each( function( i ) {
				jQuery( this ).wrapInner( html.call( this, i ) );
			} );
		}

		return this.each( function() {
			var self = jQuery( this ),
				contents = self.contents();

			if ( contents.length ) {
				contents.wrapAll( html );

			} else {
				self.append( html );
			}
		} );
	},

	wrap: function( html ) {
		var htmlIsFunction = isFunction( html );

		return this.each( function( i ) {
			jQuery( this ).wrapAll( htmlIsFunction ? html.call( this, i ) : html );
		} );
	},

	unwrap: function( selector ) {
		this.parent( selector ).not( "body" ).each( function() {
			jQuery( this ).replaceWith( this.childNodes );
		} );
		return this;
	}
} );


jQuery.expr.pseudos.hidden = function( elem ) {
	return !jQuery.expr.pseudos.visible( elem );
};
jQuery.expr.pseudos.visible = function( elem ) {
	return !!( elem.offsetWidth || elem.offsetHeight || elem.getClientRects().length );
};




jQuery.ajaxSettings.xhr = function() {
	try {
		return new window.XMLHttpRequest();
	} catch ( e ) {}
};

var xhrSuccessStatus = {

		// File protocol always yields status code 0, assume 200
		0: 200,

		// Support: IE <=9 only
		// #1450: sometimes IE returns 1223 when it should be 204
		1223: 204
	},
	xhrSupported = jQuery.ajaxSettings.xhr();

support.cors = !!xhrSupported && ( "withCredentials" in xhrSupported );
support.ajax = xhrSupported = !!xhrSupported;

jQuery.ajaxTransport( function( options ) {
	var callback, errorCallback;

	// Cross domain only allowed if supported through XMLHttpRequest
	if ( support.cors || xhrSupported && !options.crossDomain ) {
		return {
			send: function( headers, complete ) {
				var i,
					xhr = options.xhr();

				xhr.open(
					options.type,
					options.url,
					options.async,
					options.username,
					options.password
				);

				// Apply custom fields if provided
				if ( options.xhrFields ) {
					for ( i in options.xhrFields ) {
						xhr[ i ] = options.xhrFields[ i ];
					}
				}

				// Override mime type if needed
				if ( options.mimeType && xhr.overrideMimeType ) {
					xhr.overrideMimeType( options.mimeType );
				}

				// X-Requested-With header
				// For cross-domain requests, seeing as conditions for a preflight are
				// akin to a jigsaw puzzle, we simply never set it to be sure.
				// (it can always be set on a per-request basis or even using ajaxSetup)
				// For same-domain requests, won't change header if already provided.
				if ( !options.crossDomain && !headers[ "X-Requested-With" ] ) {
					headers[ "X-Requested-With" ] = "XMLHttpRequest";
				}

				// Set headers
				for ( i in headers ) {
					xhr.setRequestHeader( i, headers[ i ] );
				}

				// Callback
				callback = function( type ) {
					return function() {
						if ( callback ) {
							callback = errorCallback = xhr.onload =
								xhr.onerror = xhr.onabort = xhr.ontimeout =
									xhr.onreadystatechange = null;

							if ( type === "abort" ) {
								xhr.abort();
							} else if ( type === "error" ) {

								// Support: IE <=9 only
								// On a manual native abort, IE9 throws
								// errors on any property access that is not readyState
								if ( typeof xhr.status !== "number" ) {
									complete( 0, "error" );
								} else {
									complete(

										// File: protocol always yields status 0; see #8605, #14207
										xhr.status,
										xhr.statusText
									);
								}
							} else {
								complete(
									xhrSuccessStatus[ xhr.status ] || xhr.status,
									xhr.statusText,

									// Support: IE <=9 only
									// IE9 has no XHR2 but throws on binary (trac-11426)
									// For XHR2 non-text, let the caller handle it (gh-2498)
									( xhr.responseType || "text" ) !== "text"  ||
									typeof xhr.responseText !== "string" ?
										{ binary: xhr.response } :
										{ text: xhr.responseText },
									xhr.getAllResponseHeaders()
								);
							}
						}
					};
				};

				// Listen to events
				xhr.onload = callback();
				errorCallback = xhr.onerror = xhr.ontimeout = callback( "error" );

				// Support: IE 9 only
				// Use onreadystatechange to replace onabort
				// to handle uncaught aborts
				if ( xhr.onabort !== undefined ) {
					xhr.onabort = errorCallback;
				} else {
					xhr.onreadystatechange = function() {

						// Check readyState before timeout as it changes
						if ( xhr.readyState === 4 ) {

							// Allow onerror to be called first,
							// but that will not handle a native abort
							// Also, save errorCallback to a variable
							// as xhr.onerror cannot be accessed
							window.setTimeout( function() {
								if ( callback ) {
									errorCallback();
								}
							} );
						}
					};
				}

				// Create the abort callback
				callback = callback( "abort" );

				try {

					// Do send the request (this may raise an exception)
					xhr.send( options.hasContent && options.data || null );
				} catch ( e ) {

					// #14683: Only rethrow if this hasn't been notified as an error yet
					if ( callback ) {
						throw e;
					}
				}
			},

			abort: function() {
				if ( callback ) {
					callback();
				}
			}
		};
	}
} );




// Prevent auto-execution of scripts when no explicit dataType was provided (See gh-2432)
jQuery.ajaxPrefilter( function( s ) {
	if ( s.crossDomain ) {
		s.contents.script = false;
	}
} );

// Install script dataType
jQuery.ajaxSetup( {
	accepts: {
		script: "text/javascript, application/javascript, " +
			"application/ecmascript, application/x-ecmascript"
	},
	contents: {
		script: /\b(?:java|ecma)script\b/
	},
	converters: {
		"text script": function( text ) {
			jQuery.globalEval( text );
			return text;
		}
	}
} );

// Handle cache's special case and crossDomain
jQuery.ajaxPrefilter( "script", function( s ) {
	if ( s.cache === undefined ) {
		s.cache = false;
	}
	if ( s.crossDomain ) {
		s.type = "GET";
	}
} );

// Bind script tag hack transport
jQuery.ajaxTransport( "script", function( s ) {

	// This transport only deals with cross domain or forced-by-attrs requests
	if ( s.crossDomain || s.scriptAttrs ) {
		var script, callback;
		return {
			send: function( _, complete ) {
				script = jQuery( "<script>" )
					.attr( s.scriptAttrs || {} )
					.prop( { charset: s.scriptCharset, src: s.url } )
					.on( "load error", callback = function( evt ) {
						script.remove();
						callback = null;
						if ( evt ) {
							complete( evt.type === "error" ? 404 : 200, evt.type );
						}
					} );

				// Use native DOM manipulation to avoid our domManip AJAX trickery
				document.head.appendChild( script[ 0 ] );
			},
			abort: function() {
				if ( callback ) {
					callback();
				}
			}
		};
	}
} );




var oldCallbacks = [],
	rjsonp = /(=)\?(?=&|$)|\?\?/;

// Default jsonp settings
jQuery.ajaxSetup( {
	jsonp: "callback",
	jsonpCallback: function() {
		var callback = oldCallbacks.pop() || ( jQuery.expando + "_" + ( nonce.guid++ ) );
		this[ callback ] = true;
		return callback;
	}
} );

// Detect, normalize options and install callbacks for jsonp requests
jQuery.ajaxPrefilter( "json jsonp", function( s, originalSettings, jqXHR ) {

	var callbackName, overwritten, responseContainer,
		jsonProp = s.jsonp !== false && ( rjsonp.test( s.url ) ?
			"url" :
			typeof s.data === "string" &&
				( s.contentType || "" )
					.indexOf( "application/x-www-form-urlencoded" ) === 0 &&
				rjsonp.test( s.data ) && "data"
		);

	// Handle iff the expected data type is "jsonp" or we have a parameter to set
	if ( jsonProp || s.dataTypes[ 0 ] === "jsonp" ) {

		// Get callback name, remembering preexisting value associated with it
		callbackName = s.jsonpCallback = isFunction( s.jsonpCallback ) ?
			s.jsonpCallback() :
			s.jsonpCallback;

		// Insert callback into url or form data
		if ( jsonProp ) {
			s[ jsonProp ] = s[ jsonProp ].replace( rjsonp, "$1" + callbackName );
		} else if ( s.jsonp !== false ) {
			s.url += ( rquery.test( s.url ) ? "&" : "?" ) + s.jsonp + "=" + callbackName;
		}

		// Use data converter to retrieve json after script execution
		s.converters[ "script json" ] = function() {
			if ( !responseContainer ) {
				jQuery.error( callbackName + " was not called" );
			}
			return responseContainer[ 0 ];
		};

		// Force json dataType
		s.dataTypes[ 0 ] = "json";

		// Install callback
		overwritten = window[ callbackName ];
		window[ callbackName ] = function() {
			responseContainer = arguments;
		};

		// Clean-up function (fires after converters)
		jqXHR.always( function() {

			// If previous value didn't exist - remove it
			if ( overwritten === undefined ) {
				jQuery( window ).removeProp( callbackName );

			// Otherwise restore preexisting value
			} else {
				window[ callbackName ] = overwritten;
			}

			// Save back as free
			if ( s[ callbackName ] ) {

				// Make sure that re-using the options doesn't screw things around
				s.jsonpCallback = originalSettings.jsonpCallback;

				// Save the callback name for future use
				oldCallbacks.push( callbackName );
			}

			// Call if it was a function and we have a response
			if ( responseContainer && isFunction( overwritten ) ) {
				overwritten( responseContainer[ 0 ] );
			}

			responseContainer = overwritten = undefined;
		} );

		// Delegate to script
		return "script";
	}
} );




// Support: Safari 8 only
// In Safari 8 documents created via document.implementation.createHTMLDocument
// collapse sibling forms: the second one becomes a child of the first one.
// Because of that, this security measure has to be disabled in Safari 8.
// https://bugs.webkit.org/show_bug.cgi?id=137337
support.createHTMLDocument = ( function() {
	var body = document.implementation.createHTMLDocument( "" ).body;
	body.innerHTML = "<form></form><form></form>";
	return body.childNodes.length === 2;
} )();


// Argument "data" should be string of html
// context (optional): If specified, the fragment will be created in this context,
// defaults to document
// keepScripts (optional): If true, will include scripts passed in the html string
jQuery.parseHTML = function( data, context, keepScripts ) {
	if ( typeof data !== "string" ) {
		return [];
	}
	if ( typeof context === "boolean" ) {
		keepScripts = context;
		context = false;
	}

	var base, parsed, scripts;

	if ( !context ) {

		// Stop scripts or inline event handlers from being executed immediately
		// by using document.implementation
		if ( support.createHTMLDocument ) {
			context = document.implementation.createHTMLDocument( "" );

			// Set the base href for the created document
			// so any parsed elements with URLs
			// are based on the document's URL (gh-2965)
			base = context.createElement( "base" );
			base.href = document.location.href;
			context.head.appendChild( base );
		} else {
			context = document;
		}
	}

	parsed = rsingleTag.exec( data );
	scripts = !keepScripts && [];

	// Single tag
	if ( parsed ) {
		return [ context.createElement( parsed[ 1 ] ) ];
	}

	parsed = buildFragment( [ data ], context, scripts );

	if ( scripts && scripts.length ) {
		jQuery( scripts ).remove();
	}

	return jQuery.merge( [], parsed.childNodes );
};


/**
 * Load a url into a page
 */
jQuery.fn.load = function( url, params, callback ) {
	var selector, type, response,
		self = this,
		off = url.indexOf( " " );

	if ( off > -1 ) {
		selector = stripAndCollapse( url.slice( off ) );
		url = url.slice( 0, off );
	}

	// If it's a function
	if ( isFunction( params ) ) {

		// We assume that it's the callback
		callback = params;
		params = undefined;

	// Otherwise, build a param string
	} else if ( params && typeof params === "object" ) {
		type = "POST";
	}

	// If we have elements to modify, make the request
	if ( self.length > 0 ) {
		jQuery.ajax( {
			url: url,

			// If "type" variable is undefined, then "GET" method will be used.
			// Make value of this field explicit since
			// user can override it through ajaxSetup method
			type: type || "GET",
			dataType: "html",
			data: params
		} ).done( function( responseText ) {

			// Save response for use in complete callback
			response = arguments;

			self.html( selector ?

				// If a selector was specified, locate the right elements in a dummy div
				// Exclude scripts to avoid IE 'Permission Denied' errors
				jQuery( "<div>" ).append( jQuery.parseHTML( responseText ) ).find( selector ) :

				// Otherwise use the full result
				responseText );

		// If the request succeeds, this function gets "data", "status", "jqXHR"
		// but they are ignored because response was set above.
		// If it fails, this function gets "jqXHR", "status", "error"
		} ).always( callback && function( jqXHR, status ) {
			self.each( function() {
				callback.apply( this, response || [ jqXHR.responseText, status, jqXHR ] );
			} );
		} );
	}

	return this;
};




jQuery.expr.pseudos.animated = function( elem ) {
	return jQuery.grep( jQuery.timers, function( fn ) {
		return elem === fn.elem;
	} ).length;
};




jQuery.offset = {
	setOffset: function( elem, options, i ) {
		var curPosition, curLeft, curCSSTop, curTop, curOffset, curCSSLeft, calculatePosition,
			position = jQuery.css( elem, "position" ),
			curElem = jQuery( elem ),
			props = {};

		// Set position first, in-case top/left are set even on static elem
		if ( position === "static" ) {
			elem.style.position = "relative";
		}

		curOffset = curElem.offset();
		curCSSTop = jQuery.css( elem, "top" );
		curCSSLeft = jQuery.css( elem, "left" );
		calculatePosition = ( position === "absolute" || position === "fixed" ) &&
			( curCSSTop + curCSSLeft ).indexOf( "auto" ) > -1;

		// Need to be able to calculate position if either
		// top or left is auto and position is either absolute or fixed
		if ( calculatePosition ) {
			curPosition = curElem.position();
			curTop = curPosition.top;
			curLeft = curPosition.left;

		} else {
			curTop = parseFloat( curCSSTop ) || 0;
			curLeft = parseFloat( curCSSLeft ) || 0;
		}

		if ( isFunction( options ) ) {

			// Use jQuery.extend here to allow modification of coordinates argument (gh-1848)
			options = options.call( elem, i, jQuery.extend( {}, curOffset ) );
		}

		if ( options.top != null ) {
			props.top = ( options.top - curOffset.top ) + curTop;
		}
		if ( options.left != null ) {
			props.left = ( options.left - curOffset.left ) + curLeft;
		}

		if ( "using" in options ) {
			options.using.call( elem, props );

		} else {
			curElem.css( props );
		}
	}
};

jQuery.fn.extend( {

	// offset() relates an element's border box to the document origin
	offset: function( options ) {

		// Preserve chaining for setter
		if ( arguments.length ) {
			return options === undefined ?
				this :
				this.each( function( i ) {
					jQuery.offset.setOffset( this, options, i );
				} );
		}

		var rect, win,
			elem = this[ 0 ];

		if ( !elem ) {
			return;
		}

		// Return zeros for disconnected and hidden (display: none) elements (gh-2310)
		// Support: IE <=11 only
		// Running getBoundingClientRect on a
		// disconnected node in IE throws an error
		if ( !elem.getClientRects().length ) {
			return { top: 0, left: 0 };
		}

		// Get document-relative position by adding viewport scroll to viewport-relative gBCR
		rect = elem.getBoundingClientRect();
		win = elem.ownerDocument.defaultView;
		return {
			top: rect.top + win.pageYOffset,
			left: rect.left + win.pageXOffset
		};
	},

	// position() relates an element's margin box to its offset parent's padding box
	// This corresponds to the behavior of CSS absolute positioning
	position: function() {
		if ( !this[ 0 ] ) {
			return;
		}

		var offsetParent, offset, doc,
			elem = this[ 0 ],
			parentOffset = { top: 0, left: 0 };

		// position:fixed elements are offset from the viewport, which itself always has zero offset
		if ( jQuery.css( elem, "position" ) === "fixed" ) {

			// Assume position:fixed implies availability of getBoundingClientRect
			offset = elem.getBoundingClientRect();

		} else {
			offset = this.offset();

			// Account for the *real* offset parent, which can be the document or its root element
			// when a statically positioned element is identified
			doc = elem.ownerDocument;
			offsetParent = elem.offsetParent || doc.documentElement;
			while ( offsetParent &&
				( offsetParent === doc.body || offsetParent === doc.documentElement ) &&
				jQuery.css( offsetParent, "position" ) === "static" ) {

				offsetParent = offsetParent.parentNode;
			}
			if ( offsetParent && offsetParent !== elem && offsetParent.nodeType === 1 ) {

				// Incorporate borders into its offset, since they are outside its content origin
				parentOffset = jQuery( offsetParent ).offset();
				parentOffset.top += jQuery.css( offsetParent, "borderTopWidth", true );
				parentOffset.left += jQuery.css( offsetParent, "borderLeftWidth", true );
			}
		}

		// Subtract parent offsets and element margins
		return {
			top: offset.top - parentOffset.top - jQuery.css( elem, "marginTop", true ),
			left: offset.left - parentOffset.left - jQuery.css( elem, "marginLeft", true )
		};
	},

	// This method will return documentElement in the following cases:
	// 1) For the element inside the iframe without offsetParent, this method will return
	//    documentElement of the parent window
	// 2) For the hidden or detached element
	// 3) For body or html element, i.e. in case of the html node - it will return itself
	//
	// but those exceptions were never presented as a real life use-cases
	// and might be considered as more preferable results.
	//
	// This logic, however, is not guaranteed and can change at any point in the future
	offsetParent: function() {
		return this.map( function() {
			var offsetParent = this.offsetParent;

			while ( offsetParent && jQuery.css( offsetParent, "position" ) === "static" ) {
				offsetParent = offsetParent.offsetParent;
			}

			return offsetParent || documentElement;
		} );
	}
} );

// Create scrollLeft and scrollTop methods
jQuery.each( { scrollLeft: "pageXOffset", scrollTop: "pageYOffset" }, function( method, prop ) {
	var top = "pageYOffset" === prop;

	jQuery.fn[ method ] = function( val ) {
		return access( this, function( elem, method, val ) {

			// Coalesce documents and windows
			var win;
			if ( isWindow( elem ) ) {
				win = elem;
			} else if ( elem.nodeType === 9 ) {
				win = elem.defaultView;
			}

			if ( val === undefined ) {
				return win ? win[ prop ] : elem[ method ];
			}

			if ( win ) {
				win.scrollTo(
					!top ? val : win.pageXOffset,
					top ? val : win.pageYOffset
				);

			} else {
				elem[ method ] = val;
			}
		}, method, val, arguments.length );
	};
} );

// Support: Safari <=7 - 9.1, Chrome <=37 - 49
// Add the top/left cssHooks using jQuery.fn.position
// Webkit bug: https://bugs.webkit.org/show_bug.cgi?id=29084
// Blink bug: https://bugs.chromium.org/p/chromium/issues/detail?id=589347
// getComputedStyle returns percent when specified for top/left/bottom/right;
// rather than make the css module depend on the offset module, just check for it here
jQuery.each( [ "top", "left" ], function( _i, prop ) {
	jQuery.cssHooks[ prop ] = addGetHookIf( support.pixelPosition,
		function( elem, computed ) {
			if ( computed ) {
				computed = curCSS( elem, prop );

				// If curCSS returns percentage, fallback to offset
				return rnumnonpx.test( computed ) ?
					jQuery( elem ).position()[ prop ] + "px" :
					computed;
			}
		}
	);
} );


// Create innerHeight, innerWidth, height, width, outerHeight and outerWidth methods
jQuery.each( { Height: "height", Width: "width" }, function( name, type ) {
	jQuery.each( {
		padding: "inner" + name,
		content: type,
		"": "outer" + name
	}, function( defaultExtra, funcName ) {

		// Margin is only for outerHeight, outerWidth
		jQuery.fn[ funcName ] = function( margin, value ) {
			var chainable = arguments.length && ( defaultExtra || typeof margin !== "boolean" ),
				extra = defaultExtra || ( margin === true || value === true ? "margin" : "border" );

			return access( this, function( elem, type, value ) {
				var doc;

				if ( isWindow( elem ) ) {

					// $( window ).outerWidth/Height return w/h including scrollbars (gh-1729)
					return funcName.indexOf( "outer" ) === 0 ?
						elem[ "inner" + name ] :
						elem.document.documentElement[ "client" + name ];
				}

				// Get document width or height
				if ( elem.nodeType === 9 ) {
					doc = elem.documentElement;

					// Either scroll[Width/Height] or offset[Width/Height] or client[Width/Height],
					// whichever is greatest
					return Math.max(
						elem.body[ "scroll" + name ], doc[ "scroll" + name ],
						elem.body[ "offset" + name ], doc[ "offset" + name ],
						doc[ "client" + name ]
					);
				}

				return value === undefined ?

					// Get width or height on the element, requesting but not forcing parseFloat
					jQuery.css( elem, type, extra ) :

					// Set width or height on the element
					jQuery.style( elem, type, value, extra );
			}, type, chainable ? margin : undefined, chainable );
		};
	} );
} );


jQuery.each( [
	"ajaxStart",
	"ajaxStop",
	"ajaxComplete",
	"ajaxError",
	"ajaxSuccess",
	"ajaxSend"
], function( _i, type ) {
	jQuery.fn[ type ] = function( fn ) {
		return this.on( type, fn );
	};
} );




jQuery.fn.extend( {

	bind: function( types, data, fn ) {
		return this.on( types, null, data, fn );
	},
	unbind: function( types, fn ) {
		return this.off( types, null, fn );
	},

	delegate: function( selector, types, data, fn ) {
		return this.on( types, selector, data, fn );
	},
	undelegate: function( selector, types, fn ) {

		// ( namespace ) or ( selector, types [, fn] )
		return arguments.length === 1 ?
			this.off( selector, "**" ) :
			this.off( types, selector || "**", fn );
	},

	hover: function( fnOver, fnOut ) {
		return this.mouseenter( fnOver ).mouseleave( fnOut || fnOver );
	}
} );

jQuery.each(
	( "blur focus focusin focusout resize scroll click dblclick " +
	"mousedown mouseup mousemove mouseover mouseout mouseenter mouseleave " +
	"change select submit keydown keypress keyup contextmenu" ).split( " " ),
	function( _i, name ) {

		// Handle event binding
		jQuery.fn[ name ] = function( data, fn ) {
			return arguments.length > 0 ?
				this.on( name, null, data, fn ) :
				this.trigger( name );
		};
	}
);




// Support: Android <=4.0 only
// Make sure we trim BOM and NBSP
var rtrim = /^[\s\uFEFF\xA0]+|[\s\uFEFF\xA0]+$/g;

// Bind a function to a context, optionally partially applying any
// arguments.
// jQuery.proxy is deprecated to promote standards (specifically Function#bind)
// However, it is not slated for removal any time soon
jQuery.proxy = function( fn, context ) {
	var tmp, args, proxy;

	if ( typeof context === "string" ) {
		tmp = fn[ context ];
		context = fn;
		fn = tmp;
	}

	// Quick check to determine if target is callable, in the spec
	// this throws a TypeError, but we will just return undefined.
	if ( !isFunction( fn ) ) {
		return undefined;
	}

	// Simulated bind
	args = slice.call( arguments, 2 );
	proxy = function() {
		return fn.apply( context || this, args.concat( slice.call( arguments ) ) );
	};

	// Set the guid of unique handler to the same of original handler, so it can be removed
	proxy.guid = fn.guid = fn.guid || jQuery.guid++;

	return proxy;
};

jQuery.holdReady = function( hold ) {
	if ( hold ) {
		jQuery.readyWait++;
	} else {
		jQuery.ready( true );
	}
};
jQuery.isArray = Array.isArray;
jQuery.parseJSON = JSON.parse;
jQuery.nodeName = nodeName;
jQuery.isFunction = isFunction;
jQuery.isWindow = isWindow;
jQuery.camelCase = camelCase;
jQuery.type = toType;

jQuery.now = Date.now;

jQuery.isNumeric = function( obj ) {

	// As of jQuery 3.0, isNumeric is limited to
	// strings and numbers (primitives or objects)
	// that can be coerced to finite numbers (gh-2662)
	var type = jQuery.type( obj );
	return ( type === "number" || type === "string" ) &&

		// parseFloat NaNs numeric-cast false positives ("")
		// ...but misinterprets leading-number strings, particularly hex literals ("0x...")
		// subtraction forces infinities to NaN
		!isNaN( obj - parseFloat( obj ) );
};

jQuery.trim = function( text ) {
	return text == null ?
		"" :
		( text + "" ).replace( rtrim, "" );
};



// Register as a named AMD module, since jQuery can be concatenated with other
// files that may use define, but not via a proper concatenation script that
// understands anonymous AMD modules. A named AMD is safest and most robust
// way to register. Lowercase jquery is used because AMD module names are
// derived from file names, and jQuery is normally delivered in a lowercase
// file name. Do this after creating the global so that if an AMD module wants
// to call noConflict to hide this version of jQuery, it will work.

// Note that for maximum portability, libraries that are not jQuery should
// declare themselves as anonymous modules, and avoid setting a global if an
// AMD loader is present. jQuery is a special case. For more information, see
// https://github.com/jrburke/requirejs/wiki/Updating-existing-libraries#wiki-anon

if ( typeof define === "function" && define.amd ) {
	define( "jquery", [], function() {
		return jQuery;
	} );
}




var

	// Map over jQuery in case of overwrite
	_jQuery = window.jQuery,

	// Map over the $ in case of overwrite
	_$ = window.$;

jQuery.noConflict = function( deep ) {
	if ( window.$ === jQuery ) {
		window.$ = _$;
	}

	if ( deep && window.jQuery === jQuery ) {
		window.jQuery = _jQuery;
	}

	return jQuery;
};

// Expose jQuery and $ identifiers, even in AMD
// (#7102#comment:10, https://github.com/jquery/jquery/pull/557)
// and CommonJS for browser emulators (#13566)
if ( typeof noGlobal === "undefined" ) {
	window.jQuery = window.$ = jQuery;
}




return jQuery;
} );

//     Underscore.js 1.8.3
//     http://underscorejs.org
//     (c) 2009-2015 Jeremy Ashkenas, DocumentCloud and Investigative Reporters & Editors
//     Underscore may be freely distributed under the MIT license.

(function() {

  // Baseline setup
  // --------------

  // Establish the root object, `window` (`self`) in the browser, or `global` on the server.
  // We use `self` instead of `window` for `WebWorker` support.
  var root = typeof self === 'object' && self.self === self && self ||
            typeof global === 'object' && global.global === global && global;

  // Save the previous value of the `_` variable.
  var previousUnderscore = root._;

  // Save bytes in the minified (but not gzipped) version:
  var ArrayProto = Array.prototype, ObjProto = Object.prototype;

  // Create quick reference variables for speed access to core prototypes.
  var
    push = ArrayProto.push,
    slice = ArrayProto.slice,
    toString = ObjProto.toString,
    hasOwnProperty = ObjProto.hasOwnProperty;

  // All **ECMAScript 5** native function implementations that we hope to use
  // are declared here.
  var
    nativeIsArray = Array.isArray,
    nativeKeys = Object.keys,
    nativeCreate = Object.create;

  // Naked function reference for surrogate-prototype-swapping.
  var Ctor = function(){};

  // Create a safe reference to the Underscore object for use below.
  var _ = function(obj) {
    if (obj instanceof _) return obj;
    if (!(this instanceof _)) return new _(obj);
    this._wrapped = obj;
  };

  // Export the Underscore object for **Node.js**, with
  // backwards-compatibility for their old module API. If we're in
  // the browser, add `_` as a global object.
  if (typeof exports !== 'undefined') {
    if (typeof module !== 'undefined' && module.exports) {
      exports = module.exports = _;
    }
    exports._ = _;
  } else {
    root._ = _;
  }

  // Current version.
  _.VERSION = '1.8.3';

  // Internal function that returns an efficient (for current engines) version
  // of the passed-in callback, to be repeatedly applied in other Underscore
  // functions.
  var optimizeCb = function(func, context, argCount) {
    if (context === void 0) return func;
    switch (argCount == null ? 3 : argCount) {
      case 1: return function(value) {
        return func.call(context, value);
      };
      case 2: return function(value, other) {
        return func.call(context, value, other);
      };
      case 3: return function(value, index, collection) {
        return func.call(context, value, index, collection);
      };
      case 4: return function(accumulator, value, index, collection) {
        return func.call(context, accumulator, value, index, collection);
      };
    }
    return function() {
      return func.apply(context, arguments);
    };
  };

  // A mostly-internal function to generate callbacks that can be applied
  // to each element in a collection, returning the desired result — either
  // identity, an arbitrary callback, a property matcher, or a property accessor.
  var cb = function(value, context, argCount) {
    if (value == null) return _.identity;
    if (_.isFunction(value)) return optimizeCb(value, context, argCount);
    if (_.isObject(value)) return _.matcher(value);
    return _.property(value);
  };
  _.iteratee = function(value, context) {
    return cb(value, context, Infinity);
  };

  // Similar to ES6's rest param (http://ariya.ofilabs.com/2013/03/es6-and-rest-parameter.html)
  // This accumulates the arguments passed into an array, after a given index.
  var restArgs = function(func, startIndex) {
    startIndex = startIndex == null ? func.length - 1 : +startIndex;
    return function() {
      var length = Math.max(arguments.length - startIndex, 0);
      var rest = Array(length);
      var index;
      for (index = 0; index < length; index++) {
        rest[index] = arguments[index + startIndex];
      }
      switch (startIndex) {
        case 0: return func.call(this, rest);
        case 1: return func.call(this, arguments[0], rest);
        case 2: return func.call(this, arguments[0], arguments[1], rest);
      }
      var args = Array(startIndex + 1);
      for (index = 0; index < startIndex; index++) {
        args[index] = arguments[index];
      }
      args[startIndex] = rest;
      return func.apply(this, args);
    };
  };

  // An internal function for creating a new object that inherits from another.
  var baseCreate = function(prototype) {
    if (!_.isObject(prototype)) return {};
    if (nativeCreate) return nativeCreate(prototype);
    Ctor.prototype = prototype;
    var result = new Ctor;
    Ctor.prototype = null;
    return result;
  };

  var property = function(key) {
    return function(obj) {
      return obj == null ? void 0 : obj[key];
    };
  };

  // Helper for collection methods to determine whether a collection
  // should be iterated as an array or as an object
  // Related: http://people.mozilla.org/~jorendorff/es6-draft.html#sec-tolength
  // Avoids a very nasty iOS 8 JIT bug on ARM-64. #2094
  var MAX_ARRAY_INDEX = Math.pow(2, 53) - 1;
  var getLength = property('length');
  var isArrayLike = function(collection) {
    var length = getLength(collection);
    return typeof length == 'number' && length >= 0 && length <= MAX_ARRAY_INDEX;
  };

  // Collection Functions
  // --------------------

  // The cornerstone, an `each` implementation, aka `forEach`.
  // Handles raw objects in addition to array-likes. Treats all
  // sparse array-likes as if they were dense.
  _.each = _.forEach = function(obj, iteratee, context) {
    iteratee = optimizeCb(iteratee, context);
    var i, length;
    if (isArrayLike(obj)) {
      for (i = 0, length = obj.length; i < length; i++) {
        iteratee(obj[i], i, obj);
      }
    } else {
      var keys = _.keys(obj);
      for (i = 0, length = keys.length; i < length; i++) {
        iteratee(obj[keys[i]], keys[i], obj);
      }
    }
    return obj;
  };

  // Return the results of applying the iteratee to each element.
  _.map = _.collect = function(obj, iteratee, context) {
    iteratee = cb(iteratee, context);
    var keys = !isArrayLike(obj) && _.keys(obj),
        length = (keys || obj).length,
        results = Array(length);
    for (var index = 0; index < length; index++) {
      var currentKey = keys ? keys[index] : index;
      results[index] = iteratee(obj[currentKey], currentKey, obj);
    }
    return results;
  };

  // Create a reducing function iterating left or right.
  var createReduce = function(dir) {
    // Optimized iterator function as using arguments.length
    // in the main function will deoptimize the, see #1991.
    var reducer = function(obj, iteratee, memo, initial) {
      var keys = !isArrayLike(obj) && _.keys(obj),
          length = (keys || obj).length,
          index = dir > 0 ? 0 : length - 1;
      if (!initial) {
        memo = obj[keys ? keys[index] : index];
        index += dir;
      }
      for (; index >= 0 && index < length; index += dir) {
        var currentKey = keys ? keys[index] : index;
        memo = iteratee(memo, obj[currentKey], currentKey, obj);
      }
      return memo;
    };

    return function(obj, iteratee, memo, context) {
      var initial = arguments.length >= 3;
      return reducer(obj, optimizeCb(iteratee, context, 4), memo, initial);
    };
  };

  // **Reduce** builds up a single result from a list of values, aka `inject`,
  // or `foldl`.
  _.reduce = _.foldl = _.inject = createReduce(1);

  // The right-associative version of reduce, also known as `foldr`.
  _.reduceRight = _.foldr = createReduce(-1);

  // Return the first value which passes a truth test. Aliased as `detect`.
  _.find = _.detect = function(obj, predicate, context) {
    var key;
    if (isArrayLike(obj)) {
      key = _.findIndex(obj, predicate, context);
    } else {
      key = _.findKey(obj, predicate, context);
    }
    if (key !== void 0 && key !== -1) return obj[key];
  };

  // Return all the elements that pass a truth test.
  // Aliased as `select`.
  _.filter = _.select = function(obj, predicate, context) {
    var results = [];
    predicate = cb(predicate, context);
    _.each(obj, function(value, index, list) {
      if (predicate(value, index, list)) results.push(value);
    });
    return results;
  };

  // Return all the elements for which a truth test fails.
  _.reject = function(obj, predicate, context) {
    return _.filter(obj, _.negate(cb(predicate)), context);
  };

  // Determine whether all of the elements match a truth test.
  // Aliased as `all`.
  _.every = _.all = function(obj, predicate, context) {
    predicate = cb(predicate, context);
    var keys = !isArrayLike(obj) && _.keys(obj),
        length = (keys || obj).length;
    for (var index = 0; index < length; index++) {
      var currentKey = keys ? keys[index] : index;
      if (!predicate(obj[currentKey], currentKey, obj)) return false;
    }
    return true;
  };

  // Determine if at least one element in the object matches a truth test.
  // Aliased as `any`.
  _.some = _.any = function(obj, predicate, context) {
    predicate = cb(predicate, context);
    var keys = !isArrayLike(obj) && _.keys(obj),
        length = (keys || obj).length;
    for (var index = 0; index < length; index++) {
      var currentKey = keys ? keys[index] : index;
      if (predicate(obj[currentKey], currentKey, obj)) return true;
    }
    return false;
  };

  // Determine if the array or object contains a given item (using `===`).
  // Aliased as `includes` and `include`.
  _.contains = _.includes = _.include = function(obj, item, fromIndex, guard) {
    if (!isArrayLike(obj)) obj = _.values(obj);
    if (typeof fromIndex != 'number' || guard) fromIndex = 0;
    return _.indexOf(obj, item, fromIndex) >= 0;
  };

  // Invoke a method (with arguments) on every item in a collection.
  _.invoke = restArgs(function(obj, method, args) {
    var isFunc = _.isFunction(method);
    return _.map(obj, function(value) {
      var func = isFunc ? method : value[method];
      return func == null ? func : func.apply(value, args);
    });
  });

  // Convenience version of a common use case of `map`: fetching a property.
  _.pluck = function(obj, key) {
    return _.map(obj, _.property(key));
  };

  // Convenience version of a common use case of `filter`: selecting only objects
  // containing specific `key:value` pairs.
  _.where = function(obj, attrs) {
    return _.filter(obj, _.matcher(attrs));
  };

  // Convenience version of a common use case of `find`: getting the first object
  // containing specific `key:value` pairs.
  _.findWhere = function(obj, attrs) {
    return _.find(obj, _.matcher(attrs));
  };

  // Return the maximum element (or element-based computation).
  _.max = function(obj, iteratee, context) {
    var result = -Infinity, lastComputed = -Infinity,
        value, computed;
    if (iteratee == null && obj != null) {
      obj = isArrayLike(obj) ? obj : _.values(obj);
      for (var i = 0, length = obj.length; i < length; i++) {
        value = obj[i];
        if (value > result) {
          result = value;
        }
      }
    } else {
      iteratee = cb(iteratee, context);
      _.each(obj, function(v, index, list) {
        computed = iteratee(v, index, list);
        if (computed > lastComputed || computed === -Infinity && result === -Infinity) {
          result = v;
          lastComputed = computed;
        }
      });
    }
    return result;
  };

  // Return the minimum element (or element-based computation).
  _.min = function(obj, iteratee, context) {
    var result = Infinity, lastComputed = Infinity,
        value, computed;
    if (iteratee == null && obj != null) {
      obj = isArrayLike(obj) ? obj : _.values(obj);
      for (var i = 0, length = obj.length; i < length; i++) {
        value = obj[i];
        if (value < result) {
          result = value;
        }
      }
    } else {
      iteratee = cb(iteratee, context);
      _.each(obj, function(v, index, list) {
        computed = iteratee(v, index, list);
        if (computed < lastComputed || computed === Infinity && result === Infinity) {
          result = v;
          lastComputed = computed;
        }
      });
    }
    return result;
  };

  // Shuffle a collection.
  _.shuffle = function(obj) {
    return _.sample(obj, Infinity);
  };

  // Sample **n** random values from a collection using the modern version of the
  // [Fisher-Yates shuffle](http://en.wikipedia.org/wiki/Fisher–Yates_shuffle).
  // If **n** is not specified, returns a single random element.
  // The internal `guard` argument allows it to work with `map`.
  _.sample = function(obj, n, guard) {
    if (n == null || guard) {
      if (!isArrayLike(obj)) obj = _.values(obj);
      return obj[_.random(obj.length - 1)];
    }
    var sample = isArrayLike(obj) ? _.clone(obj) : _.values(obj);
    var length = getLength(sample);
    n = Math.max(Math.min(n, length), 0);
    var last = length - 1;
    for (var index = 0; index < n; index++) {
      var rand = _.random(index, last);
      var temp = sample[index];
      sample[index] = sample[rand];
      sample[rand] = temp;
    }
    return sample.slice(0, n);
  };

  // Sort the object's values by a criterion produced by an iteratee.
  _.sortBy = function(obj, iteratee, context) {
    iteratee = cb(iteratee, context);
    return _.pluck(_.map(obj, function(value, index, list) {
      return {
        value: value,
        index: index,
        criteria: iteratee(value, index, list)
      };
    }).sort(function(left, right) {
      var a = left.criteria;
      var b = right.criteria;
      if (a !== b) {
        if (a > b || a === void 0) return 1;
        if (a < b || b === void 0) return -1;
      }
      return left.index - right.index;
    }), 'value');
  };

  // An internal function used for aggregate "group by" operations.
  var group = function(behavior, partition) {
    return function(obj, iteratee, context) {
      var result = partition ? [[], []] : {};
      iteratee = cb(iteratee, context);
      _.each(obj, function(value, index) {
        var key = iteratee(value, index, obj);
        behavior(result, value, key);
      });
      return result;
    };
  };

  // Groups the object's values by a criterion. Pass either a string attribute
  // to group by, or a function that returns the criterion.
  _.groupBy = group(function(result, value, key) {
    if (_.has(result, key)) result[key].push(value); else result[key] = [value];
  });

  // Indexes the object's values by a criterion, similar to `groupBy`, but for
  // when you know that your index values will be unique.
  _.indexBy = group(function(result, value, key) {
    result[key] = value;
  });

  // Counts instances of an object that group by a certain criterion. Pass
  // either a string attribute to count by, or a function that returns the
  // criterion.
  _.countBy = group(function(result, value, key) {
    if (_.has(result, key)) result[key]++; else result[key] = 1;
  });

  // Safely create a real, live array from anything iterable.
  _.toArray = function(obj) {
    if (!obj) return [];
    if (_.isArray(obj)) return slice.call(obj);
    if (isArrayLike(obj)) return _.map(obj, _.identity);
    return _.values(obj);
  };

  // Return the number of elements in an object.
  _.size = function(obj) {
    if (obj == null) return 0;
    return isArrayLike(obj) ? obj.length : _.keys(obj).length;
  };

  // Split a collection into two arrays: one whose elements all satisfy the given
  // predicate, and one whose elements all do not satisfy the predicate.
  _.partition = group(function(result, value, pass) {
    result[pass ? 0 : 1].push(value);
  }, true);

  // Array Functions
  // ---------------

  // Get the first element of an array. Passing **n** will return the first N
  // values in the array. Aliased as `head` and `take`. The **guard** check
  // allows it to work with `_.map`.
  _.first = _.head = _.take = function(array, n, guard) {
    if (array == null) return void 0;
    if (n == null || guard) return array[0];
    return _.initial(array, array.length - n);
  };

  // Returns everything but the last entry of the array. Especially useful on
  // the arguments object. Passing **n** will return all the values in
  // the array, excluding the last N.
  _.initial = function(array, n, guard) {
    return slice.call(array, 0, Math.max(0, array.length - (n == null || guard ? 1 : n)));
  };

  // Get the last element of an array. Passing **n** will return the last N
  // values in the array.
  _.last = function(array, n, guard) {
    if (array == null) return void 0;
    if (n == null || guard) return array[array.length - 1];
    return _.rest(array, Math.max(0, array.length - n));
  };

  // Returns everything but the first entry of the array. Aliased as `tail` and `drop`.
  // Especially useful on the arguments object. Passing an **n** will return
  // the rest N values in the array.
  _.rest = _.tail = _.drop = function(array, n, guard) {
    return slice.call(array, n == null || guard ? 1 : n);
  };

  // Trim out all falsy values from an array.
  _.compact = function(array) {
    return _.filter(array, _.identity);
  };

  // Internal implementation of a recursive `flatten` function.
  var flatten = function(input, shallow, strict, output) {
    output = output || [];
    var idx = output.length;
    for (var i = 0, length = getLength(input); i < length; i++) {
      var value = input[i];
      if (isArrayLike(value) && (_.isArray(value) || _.isArguments(value))) {
        //flatten current level of array or arguments object
        if (shallow) {
          var j = 0, len = value.length;
          while (j < len) output[idx++] = value[j++];
        } else {
          flatten(value, shallow, strict, output);
          idx = output.length;
        }
      } else if (!strict) {
        output[idx++] = value;
      }
    }
    return output;
  };

  // Flatten out an array, either recursively (by default), or just one level.
  _.flatten = function(array, shallow) {
    return flatten(array, shallow, false);
  };

  // Return a version of the array that does not contain the specified value(s).
  _.without = restArgs(function(array, otherArrays) {
    return _.difference(array, otherArrays);
  });

  // Produce a duplicate-free version of the array. If the array has already
  // been sorted, you have the option of using a faster algorithm.
  // Aliased as `unique`.
  _.uniq = _.unique = function(array, isSorted, iteratee, context) {
    if (!_.isBoolean(isSorted)) {
      context = iteratee;
      iteratee = isSorted;
      isSorted = false;
    }
    if (iteratee != null) iteratee = cb(iteratee, context);
    var result = [];
    var seen = [];
    for (var i = 0, length = getLength(array); i < length; i++) {
      var value = array[i],
          computed = iteratee ? iteratee(value, i, array) : value;
      if (isSorted) {
        if (!i || seen !== computed) result.push(value);
        seen = computed;
      } else if (iteratee) {
        if (!_.contains(seen, computed)) {
          seen.push(computed);
          result.push(value);
        }
      } else if (!_.contains(result, value)) {
        result.push(value);
      }
    }
    return result;
  };

  // Produce an array that contains the union: each distinct element from all of
  // the passed-in arrays.
  _.union = restArgs(function(arrays) {
    return _.uniq(flatten(arrays, true, true));
  });

  // Produce an array that contains every item shared between all the
  // passed-in arrays.
  _.intersection = function(array) {
    var result = [];
    var argsLength = arguments.length;
    for (var i = 0, length = getLength(array); i < length; i++) {
      var item = array[i];
      if (_.contains(result, item)) continue;
      var j;
      for (j = 1; j < argsLength; j++) {
        if (!_.contains(arguments[j], item)) break;
      }
      if (j === argsLength) result.push(item);
    }
    return result;
  };

  // Take the difference between one array and a number of other arrays.
  // Only the elements present in just the first array will remain.
  _.difference = restArgs(function(array, rest) {
    rest = flatten(rest, true, true);
    return _.filter(array, function(value){
      return !_.contains(rest, value);
    });
  });

  // Complement of _.zip. Unzip accepts an array of arrays and groups
  // each array's elements on shared indices
  _.unzip = function(array) {
    var length = array && _.max(array, getLength).length || 0;
    var result = Array(length);

    for (var index = 0; index < length; index++) {
      result[index] = _.pluck(array, index);
    }
    return result;
  };

  // Zip together multiple lists into a single array -- elements that share
  // an index go together.
  _.zip = restArgs(_.unzip);

  // Converts lists into objects. Pass either a single array of `[key, value]`
  // pairs, or two parallel arrays of the same length -- one of keys, and one of
  // the corresponding values.
  _.object = function(list, values) {
    var result = {};
    for (var i = 0, length = getLength(list); i < length; i++) {
      if (values) {
        result[list[i]] = values[i];
      } else {
        result[list[i][0]] = list[i][1];
      }
    }
    return result;
  };

  // Generator function to create the findIndex and findLastIndex functions
  var createPredicateIndexFinder = function(dir) {
    return function(array, predicate, context) {
      predicate = cb(predicate, context);
      var length = getLength(array);
      var index = dir > 0 ? 0 : length - 1;
      for (; index >= 0 && index < length; index += dir) {
        if (predicate(array[index], index, array)) return index;
      }
      return -1;
    };
  };

  // Returns the first index on an array-like that passes a predicate test
  _.findIndex = createPredicateIndexFinder(1);
  _.findLastIndex = createPredicateIndexFinder(-1);

  // Use a comparator function to figure out the smallest index at which
  // an object should be inserted so as to maintain order. Uses binary search.
  _.sortedIndex = function(array, obj, iteratee, context) {
    iteratee = cb(iteratee, context, 1);
    var value = iteratee(obj);
    var low = 0, high = getLength(array);
    while (low < high) {
      var mid = Math.floor((low + high) / 2);
      if (iteratee(array[mid]) < value) low = mid + 1; else high = mid;
    }
    return low;
  };

  // Generator function to create the indexOf and lastIndexOf functions
  var createIndexFinder = function(dir, predicateFind, sortedIndex) {
    return function(array, item, idx) {
      var i = 0, length = getLength(array);
      if (typeof idx == 'number') {
        if (dir > 0) {
          i = idx >= 0 ? idx : Math.max(idx + length, i);
        } else {
          length = idx >= 0 ? Math.min(idx + 1, length) : idx + length + 1;
        }
      } else if (sortedIndex && idx && length) {
        idx = sortedIndex(array, item);
        return array[idx] === item ? idx : -1;
      }
      if (item !== item) {
        idx = predicateFind(slice.call(array, i, length), _.isNaN);
        return idx >= 0 ? idx + i : -1;
      }
      for (idx = dir > 0 ? i : length - 1; idx >= 0 && idx < length; idx += dir) {
        if (array[idx] === item) return idx;
      }
      return -1;
    };
  };

  // Return the position of the first occurrence of an item in an array,
  // or -1 if the item is not included in the array.
  // If the array is large and already in sort order, pass `true`
  // for **isSorted** to use binary search.
  _.indexOf = createIndexFinder(1, _.findIndex, _.sortedIndex);
  _.lastIndexOf = createIndexFinder(-1, _.findLastIndex);

  // Generate an integer Array containing an arithmetic progression. A port of
  // the native Python `range()` function. See
  // [the Python documentation](http://docs.python.org/library/functions.html#range).
  _.range = function(start, stop, step) {
    if (stop == null) {
      stop = start || 0;
      start = 0;
    }
    step = step || 1;

    var length = Math.max(Math.ceil((stop - start) / step), 0);
    var range = Array(length);

    for (var idx = 0; idx < length; idx++, start += step) {
      range[idx] = start;
    }

    return range;
  };

  // Function (ahem) Functions
  // ------------------

  // Determines whether to execute a function as a constructor
  // or a normal function with the provided arguments
  var executeBound = function(sourceFunc, boundFunc, context, callingContext, args) {
    if (!(callingContext instanceof boundFunc)) return sourceFunc.apply(context, args);
    var self = baseCreate(sourceFunc.prototype);
    var result = sourceFunc.apply(self, args);
    if (_.isObject(result)) return result;
    return self;
  };

  // Create a function bound to a given object (assigning `this`, and arguments,
  // optionally). Delegates to **ECMAScript 5**'s native `Function.bind` if
  // available.
  _.bind = restArgs(function(func, context, args) {
    if (!_.isFunction(func)) throw new TypeError('Bind must be called on a function');
    var bound = restArgs(function(callArgs) {
      return executeBound(func, bound, context, this, args.concat(callArgs));
    });
    return bound;
  });

  // Partially apply a function by creating a version that has had some of its
  // arguments pre-filled, without changing its dynamic `this` context. _ acts
  // as a placeholder by default, allowing any combination of arguments to be
  // pre-filled. Set `_.partial.placeholder` for a custom placeholder argument.
  _.partial = restArgs(function(func, boundArgs) {
    var placeholder = _.partial.placeholder;
    var bound = function() {
      var position = 0, length = boundArgs.length;
      var args = Array(length);
      for (var i = 0; i < length; i++) {
        args[i] = boundArgs[i] === placeholder ? arguments[position++] : boundArgs[i];
      }
      while (position < arguments.length) args.push(arguments[position++]);
      return executeBound(func, bound, this, this, args);
    };
    return bound;
  });

  _.partial.placeholder = _;

  // Bind a number of an object's methods to that object. Remaining arguments
  // are the method names to be bound. Useful for ensuring that all callbacks
  // defined on an object belong to it.
  _.bindAll = restArgs(function(obj, keys) {
    keys = flatten(keys, false, false);
    var index = keys.length;
    if (index < 1) throw new Error('bindAll must be passed function names');
    while (index--) {
      var key = keys[index];
      obj[key] = _.bind(obj[key], obj);
    }
  });

  // Memoize an expensive function by storing its results.
  _.memoize = function(func, hasher) {
    var memoize = function(key) {
      var cache = memoize.cache;
      var address = '' + (hasher ? hasher.apply(this, arguments) : key);
      if (!_.has(cache, address)) cache[address] = func.apply(this, arguments);
      return cache[address];
    };
    memoize.cache = {};
    return memoize;
  };

  // Delays a function for the given number of milliseconds, and then calls
  // it with the arguments supplied.
  _.delay = restArgs(function(func, wait, args) {
    return setTimeout(function(){
      return func.apply(null, args);
    }, wait);
  });

  // Defers a function, scheduling it to run after the current call stack has
  // cleared.
  _.defer = _.partial(_.delay, _, 1);

  // Returns a function, that, when invoked, will only be triggered at most once
  // during a given window of time. Normally, the throttled function will run
  // as much as it can, without ever going more than once per `wait` duration;
  // but if you'd like to disable the execution on the leading edge, pass
  // `{leading: false}`. To disable execution on the trailing edge, ditto.
  _.throttle = function(func, wait, options) {
    var context, args, result;
    var timeout = null;
    var previous = 0;
    if (!options) options = {};
    var later = function() {
      previous = options.leading === false ? 0 : _.now();
      timeout = null;
      result = func.apply(context, args);
      if (!timeout) context = args = null;
    };
    return function() {
      var now = _.now();
      if (!previous && options.leading === false) previous = now;
      var remaining = wait - (now - previous);
      context = this;
      args = arguments;
      if (remaining <= 0 || remaining > wait) {
        if (timeout) {
          clearTimeout(timeout);
          timeout = null;
        }
        previous = now;
        result = func.apply(context, args);
        if (!timeout) context = args = null;
      } else if (!timeout && options.trailing !== false) {
        timeout = setTimeout(later, remaining);
      }
      return result;
    };
  };

  // Returns a function, that, as long as it continues to be invoked, will not
  // be triggered. The function will be called after it stops being called for
  // N milliseconds. If `immediate` is passed, trigger the function on the
  // leading edge, instead of the trailing.
  _.debounce = function(func, wait, immediate) {
    var timeout, args, context, timestamp, result;

    var later = function() {
      var last = _.now() - timestamp;

      if (last < wait && last >= 0) {
        timeout = setTimeout(later, wait - last);
      } else {
        timeout = null;
        if (!immediate) {
          result = func.apply(context, args);
          if (!timeout) context = args = null;
        }
      }
    };

    return function() {
      context = this;
      args = arguments;
      timestamp = _.now();
      var callNow = immediate && !timeout;
      if (!timeout) timeout = setTimeout(later, wait);
      if (callNow) {
        result = func.apply(context, args);
        context = args = null;
      }

      return result;
    };
  };

  // Returns the first function passed as an argument to the second,
  // allowing you to adjust arguments, run code before and after, and
  // conditionally execute the original function.
  _.wrap = function(func, wrapper) {
    return _.partial(wrapper, func);
  };

  // Returns a negated version of the passed-in predicate.
  _.negate = function(predicate) {
    return function() {
      return !predicate.apply(this, arguments);
    };
  };

  // Returns a function that is the composition of a list of functions, each
  // consuming the return value of the function that follows.
  _.compose = function() {
    var args = arguments;
    var start = args.length - 1;
    return function() {
      var i = start;
      var result = args[start].apply(this, arguments);
      while (i--) result = args[i].call(this, result);
      return result;
    };
  };

  // Returns a function that will only be executed on and after the Nth call.
  _.after = function(times, func) {
    return function() {
      if (--times < 1) {
        return func.apply(this, arguments);
      }
    };
  };

  // Returns a function that will only be executed up to (but not including) the Nth call.
  _.before = function(times, func) {
    var memo;
    return function() {
      if (--times > 0) {
        memo = func.apply(this, arguments);
      }
      if (times <= 1) func = null;
      return memo;
    };
  };

  // Returns a function that will be executed at most one time, no matter how
  // often you call it. Useful for lazy initialization.
  _.once = _.partial(_.before, 2);

  _.restArgs = restArgs;

  // Object Functions
  // ----------------

  // Keys in IE < 9 that won't be iterated by `for key in ...` and thus missed.
  var hasEnumBug = !{toString: null}.propertyIsEnumerable('toString');
  var nonEnumerableProps = ['valueOf', 'isPrototypeOf', 'toString',
                      'propertyIsEnumerable', 'hasOwnProperty', 'toLocaleString'];

  var collectNonEnumProps = function(obj, keys) {
    var nonEnumIdx = nonEnumerableProps.length;
    var constructor = obj.constructor;
    var proto = _.isFunction(constructor) && constructor.prototype || ObjProto;

    // Constructor is a special case.
    var prop = 'constructor';
    if (_.has(obj, prop) && !_.contains(keys, prop)) keys.push(prop);

    while (nonEnumIdx--) {
      prop = nonEnumerableProps[nonEnumIdx];
      if (prop in obj && obj[prop] !== proto[prop] && !_.contains(keys, prop)) {
        keys.push(prop);
      }
    }
  };

  // Retrieve the names of an object's own properties.
  // Delegates to **ECMAScript 5**'s native `Object.keys`
  _.keys = function(obj) {
    if (!_.isObject(obj)) return [];
    if (nativeKeys) return nativeKeys(obj);
    var keys = [];
    for (var key in obj) if (_.has(obj, key)) keys.push(key);
    // Ahem, IE < 9.
    if (hasEnumBug) collectNonEnumProps(obj, keys);
    return keys;
  };

  // Retrieve all the property names of an object.
  _.allKeys = function(obj) {
    if (!_.isObject(obj)) return [];
    var keys = [];
    for (var key in obj) keys.push(key);
    // Ahem, IE < 9.
    if (hasEnumBug) collectNonEnumProps(obj, keys);
    return keys;
  };

  // Retrieve the values of an object's properties.
  _.values = function(obj) {
    var keys = _.keys(obj);
    var length = keys.length;
    var values = Array(length);
    for (var i = 0; i < length; i++) {
      values[i] = obj[keys[i]];
    }
    return values;
  };

  // Returns the results of applying the iteratee to each element of the object
  // In contrast to _.map it returns an object
  _.mapObject = function(obj, iteratee, context) {
    iteratee = cb(iteratee, context);
    var keys = _.keys(obj),
      length = keys.length,
      results = {};
    for (var index = 0; index < length; index++) {
      var currentKey = keys[index];
      results[currentKey] = iteratee(obj[currentKey], currentKey, obj);
    }
    return results;
  };

  // Convert an object into a list of `[key, value]` pairs.
  _.pairs = function(obj) {
    var keys = _.keys(obj);
    var length = keys.length;
    var pairs = Array(length);
    for (var i = 0; i < length; i++) {
      pairs[i] = [keys[i], obj[keys[i]]];
    }
    return pairs;
  };

  // Invert the keys and values of an object. The values must be serializable.
  _.invert = function(obj) {
    var result = {};
    var keys = _.keys(obj);
    for (var i = 0, length = keys.length; i < length; i++) {
      result[obj[keys[i]]] = keys[i];
    }
    return result;
  };

  // Return a sorted list of the function names available on the object.
  // Aliased as `methods`
  _.functions = _.methods = function(obj) {
    var names = [];
    for (var key in obj) {
      if (_.isFunction(obj[key])) names.push(key);
    }
    return names.sort();
  };

  // An internal function for creating assigner functions.
  var createAssigner = function(keysFunc, undefinedOnly) {
    return function(obj) {
      var length = arguments.length;
      if (length < 2 || obj == null) return obj;
      for (var index = 1; index < length; index++) {
        var source = arguments[index],
            keys = keysFunc(source),
            l = keys.length;
        for (var i = 0; i < l; i++) {
          var key = keys[i];
          if (!undefinedOnly || obj[key] === void 0) obj[key] = source[key];
        }
      }
      return obj;
    };
  };

  // Extend a given object with all the properties in passed-in object(s).
  _.extend = createAssigner(_.allKeys);

  // Assigns a given object with all the own properties in the passed-in object(s)
  // (https://developer.mozilla.org/docs/Web/JavaScript/Reference/Global_Objects/Object/assign)
  _.extendOwn = _.assign = createAssigner(_.keys);

  // Returns the first key on an object that passes a predicate test
  _.findKey = function(obj, predicate, context) {
    predicate = cb(predicate, context);
    var keys = _.keys(obj), key;
    for (var i = 0, length = keys.length; i < length; i++) {
      key = keys[i];
      if (predicate(obj[key], key, obj)) return key;
    }
  };

  // Internal pick helper function to determine if `obj` has key `key`.
  var keyInObj = function(value, key, obj) {
    return key in obj;
  };

  // Return a copy of the object only containing the whitelisted properties.
  _.pick = restArgs(function(obj, keys) {
    var result = {}, iteratee = keys[0];
    if (obj == null) return result;
    if (_.isFunction(iteratee)) {
      if (keys.length > 1) iteratee = optimizeCb(iteratee, keys[1]);
      keys = _.allKeys(obj);
    } else {
      iteratee = keyInObj;
      keys = flatten(keys, false, false);
      obj = Object(obj);
    }
    for (var i = 0, length = keys.length; i < length; i++) {
      var key = keys[i];
      var value = obj[key];
      if (iteratee(value, key, obj)) result[key] = value;
    }
    return result;
  });

   // Return a copy of the object without the blacklisted properties.
  _.omit = restArgs(function(obj, keys) {
    var iteratee = keys[0], context;
    if (_.isFunction(iteratee)) {
      iteratee = _.negate(iteratee);
      if (keys.length > 1) context = keys[1];
    } else {
      keys = _.map(flatten(keys, false, false), String);
      iteratee = function(value, key) {
        return !_.contains(keys, key);
      };
    }
    return _.pick(obj, iteratee, context);
  });

  // Fill in a given object with default properties.
  _.defaults = createAssigner(_.allKeys, true);

  // Creates an object that inherits from the given prototype object.
  // If additional properties are provided then they will be added to the
  // created object.
  _.create = function(prototype, props) {
    var result = baseCreate(prototype);
    if (props) _.extendOwn(result, props);
    return result;
  };

  // Create a (shallow-cloned) duplicate of an object.
  _.clone = function(obj) {
    if (!_.isObject(obj)) return obj;
    return _.isArray(obj) ? obj.slice() : _.extend({}, obj);
  };

  // Invokes interceptor with the obj, and then returns obj.
  // The primary purpose of this method is to "tap into" a method chain, in
  // order to perform operations on intermediate results within the chain.
  _.tap = function(obj, interceptor) {
    interceptor(obj);
    return obj;
  };

  // Returns whether an object has a given set of `key:value` pairs.
  _.isMatch = function(object, attrs) {
    var keys = _.keys(attrs), length = keys.length;
    if (object == null) return !length;
    var obj = Object(object);
    for (var i = 0; i < length; i++) {
      var key = keys[i];
      if (attrs[key] !== obj[key] || !(key in obj)) return false;
    }
    return true;
  };


  // Internal recursive comparison function for `isEqual`.
  var eq, deepEq;
  eq = function(a, b, aStack, bStack) {
    // Identical objects are equal. `0 === -0`, but they aren't identical.
    // See the [Harmony `egal` proposal](http://wiki.ecmascript.org/doku.php?id=harmony:egal).
    if (a === b) return a !== 0 || 1 / a === 1 / b;
    // A strict comparison is necessary because `null == undefined`.
    if (a == null || b == null) return a === b;
    // `NaN`s are equivalent, but non-reflexive.
    if (a !== a) return b !== b;
    // Exhaust primitive checks
    var type = typeof a;
    if (type !== 'function' && type !== 'object' && typeof b !== 'object') return false;
    return deepEq(a, b, aStack, bStack);
  };

  // Internal recursive comparison function for `isEqual`.
  deepEq = function(a, b, aStack, bStack) {
    // Unwrap any wrapped objects.
    if (a instanceof _) a = a._wrapped;
    if (b instanceof _) b = b._wrapped;
    // Compare `[[Class]]` names.
    var className = toString.call(a);
    if (className !== toString.call(b)) return false;
    switch (className) {
      // Strings, numbers, regular expressions, dates, and booleans are compared by value.
      case '[object RegExp]':
      // RegExps are coerced to strings for comparison (Note: '' + /a/i === '/a/i')
      case '[object String]':
        // Primitives and their corresponding object wrappers are equivalent; thus, `"5"` is
        // equivalent to `new String("5")`.
        return '' + a === '' + b;
      case '[object Number]':
        // `NaN`s are equivalent, but non-reflexive.
        // Object(NaN) is equivalent to NaN
        if (+a !== +a) return +b !== +b;
        // An `egal` comparison is performed for other numeric values.
        return +a === 0 ? 1 / +a === 1 / b : +a === +b;
      case '[object Date]':
      case '[object Boolean]':
        // Coerce dates and booleans to numeric primitive values. Dates are compared by their
        // millisecond representations. Note that invalid dates with millisecond representations
        // of `NaN` are not equivalent.
        return +a === +b;
    }

    var areArrays = className === '[object Array]';
    if (!areArrays) {
      if (typeof a != 'object' || typeof b != 'object') return false;

      // Objects with different constructors are not equivalent, but `Object`s or `Array`s
      // from different frames are.
      var aCtor = a.constructor, bCtor = b.constructor;
      if (aCtor !== bCtor && !(_.isFunction(aCtor) && aCtor instanceof aCtor &&
                               _.isFunction(bCtor) && bCtor instanceof bCtor)
                          && ('constructor' in a && 'constructor' in b)) {
        return false;
      }
    }
    // Assume equality for cyclic structures. The algorithm for detecting cyclic
    // structures is adapted from ES 5.1 section 15.12.3, abstract operation `JO`.

    // Initializing stack of traversed objects.
    // It's done here since we only need them for objects and arrays comparison.
    aStack = aStack || [];
    bStack = bStack || [];
    var length = aStack.length;
    while (length--) {
      // Linear search. Performance is inversely proportional to the number of
      // unique nested structures.
      if (aStack[length] === a) return bStack[length] === b;
    }

    // Add the first object to the stack of traversed objects.
    aStack.push(a);
    bStack.push(b);

    // Recursively compare objects and arrays.
    if (areArrays) {
      // Compare array lengths to determine if a deep comparison is necessary.
      length = a.length;
      if (length !== b.length) return false;
      // Deep compare the contents, ignoring non-numeric properties.
      while (length--) {
        if (!eq(a[length], b[length], aStack, bStack)) return false;
      }
    } else {
      // Deep compare objects.
      var keys = _.keys(a), key;
      length = keys.length;
      // Ensure that both objects contain the same number of properties before comparing deep equality.
      if (_.keys(b).length !== length) return false;
      while (length--) {
        // Deep compare each member
        key = keys[length];
        if (!(_.has(b, key) && eq(a[key], b[key], aStack, bStack))) return false;
      }
    }
    // Remove the first object from the stack of traversed objects.
    aStack.pop();
    bStack.pop();
    return true;
  };

  // Perform a deep comparison to check if two objects are equal.
  _.isEqual = function(a, b) {
    return eq(a, b);
  };

  // Is a given array, string, or object empty?
  // An "empty" object has no enumerable own-properties.
  _.isEmpty = function(obj) {
    if (obj == null) return true;
    if (isArrayLike(obj) && (_.isArray(obj) || _.isString(obj) || _.isArguments(obj))) return obj.length === 0;
    return _.keys(obj).length === 0;
  };

  // Is a given value a DOM element?
  _.isElement = function(obj) {
    return !!(obj && obj.nodeType === 1);
  };

  // Is a given value an array?
  // Delegates to ECMA5's native Array.isArray
  _.isArray = nativeIsArray || function(obj) {
    return toString.call(obj) === '[object Array]';
  };

  // Is a given variable an object?
  _.isObject = function(obj) {
    var type = typeof obj;
    return type === 'function' || type === 'object' && !!obj;
  };

  // Add some isType methods: isArguments, isFunction, isString, isNumber, isDate, isRegExp, isError.
  _.each(['Arguments', 'Function', 'String', 'Number', 'Date', 'RegExp', 'Error'], function(name) {
    _['is' + name] = function(obj) {
      return toString.call(obj) === '[object ' + name + ']';
    };
  });

  // Define a fallback version of the method in browsers (ahem, IE < 9), where
  // there isn't any inspectable "Arguments" type.
  if (!_.isArguments(arguments)) {
    _.isArguments = function(obj) {
      return _.has(obj, 'callee');
    };
  }

  // Optimize `isFunction` if appropriate. Work around some typeof bugs in old v8,
  // IE 11 (#1621), and in Safari 8 (#1929).
  if (typeof /./ != 'function' && typeof Int8Array != 'object') {
    _.isFunction = function(obj) {
      return typeof obj == 'function' || false;
    };
  }

  // Is a given object a finite number?
  _.isFinite = function(obj) {
    return isFinite(obj) && !isNaN(parseFloat(obj));
  };

  // Is the given value `NaN`? (NaN is the only number which does not equal itself).
  _.isNaN = function(obj) {
    return _.isNumber(obj) && obj !== +obj;
  };

  // Is a given value a boolean?
  _.isBoolean = function(obj) {
    return obj === true || obj === false || toString.call(obj) === '[object Boolean]';
  };

  // Is a given value equal to null?
  _.isNull = function(obj) {
    return obj === null;
  };

  // Is a given variable undefined?
  _.isUndefined = function(obj) {
    return obj === void 0;
  };

  // Shortcut function for checking if an object has a given property directly
  // on itself (in other words, not on a prototype).
  _.has = function(obj, key) {
    return obj != null && hasOwnProperty.call(obj, key);
  };

  // Utility Functions
  // -----------------

  // Run Underscore.js in *noConflict* mode, returning the `_` variable to its
  // previous owner. Returns a reference to the Underscore object.
  _.noConflict = function() {
    root._ = previousUnderscore;
    return this;
  };

  // Keep the identity function around for default iteratees.
  _.identity = function(value) {
    return value;
  };

  // Predicate-generating functions. Often useful outside of Underscore.
  _.constant = function(value) {
    return function() {
      return value;
    };
  };

  _.noop = function(){};

  _.property = property;

  // Generates a function for a given object that returns a given property.
  _.propertyOf = function(obj) {
    return obj == null ? function(){} : function(key) {
      return obj[key];
    };
  };

  // Returns a predicate for checking whether an object has a given set of
  // `key:value` pairs.
  _.matcher = _.matches = function(attrs) {
    attrs = _.extendOwn({}, attrs);
    return function(obj) {
      return _.isMatch(obj, attrs);
    };
  };

  // Run a function **n** times.
  _.times = function(n, iteratee, context) {
    var accum = Array(Math.max(0, n));
    iteratee = optimizeCb(iteratee, context, 1);
    for (var i = 0; i < n; i++) accum[i] = iteratee(i);
    return accum;
  };

  // Return a random integer between min and max (inclusive).
  _.random = function(min, max) {
    if (max == null) {
      max = min;
      min = 0;
    }
    return min + Math.floor(Math.random() * (max - min + 1));
  };

  // A (possibly faster) way to get the current timestamp as an integer.
  _.now = Date.now || function() {
    return new Date().getTime();
  };

   // List of HTML entities for escaping.
  var escapeMap = {
    '&': '&amp;',
    '<': '&lt;',
    '>': '&gt;',
    '"': '&quot;',
    "'": '&#x27;',
    '`': '&#x60;'
  };
  var unescapeMap = _.invert(escapeMap);

  // Functions for escaping and unescaping strings to/from HTML interpolation.
  var createEscaper = function(map) {
    var escaper = function(match) {
      return map[match];
    };
    // Regexes for identifying a key that needs to be escaped
    var source = '(?:' + _.keys(map).join('|') + ')';
    var testRegexp = RegExp(source);
    var replaceRegexp = RegExp(source, 'g');
    return function(string) {
      string = string == null ? '' : '' + string;
      return testRegexp.test(string) ? string.replace(replaceRegexp, escaper) : string;
    };
  };
  _.escape = createEscaper(escapeMap);
  _.unescape = createEscaper(unescapeMap);

  // If the value of the named `property` is a function then invoke it with the
  // `object` as context; otherwise, return it.
  _.result = function(object, prop, fallback) {
    var value = object == null ? void 0 : object[prop];
    if (value === void 0) {
      value = fallback;
    }
    return _.isFunction(value) ? value.call(object) : value;
  };

  // Generate a unique integer id (unique within the entire client session).
  // Useful for temporary DOM ids.
  var idCounter = 0;
  _.uniqueId = function(prefix) {
    var id = ++idCounter + '';
    return prefix ? prefix + id : id;
  };

  // By default, Underscore uses ERB-style template delimiters, change the
  // following template settings to use alternative delimiters.
  _.templateSettings = {
    evaluate: /<%([\s\S]+?)%>/g,
    interpolate: /<%=([\s\S]+?)%>/g,
    escape: /<%-([\s\S]+?)%>/g
  };

  // When customizing `templateSettings`, if you don't want to define an
  // interpolation, evaluation or escaping regex, we need one that is
  // guaranteed not to match.
  var noMatch = /(.)^/;

  // Certain characters need to be escaped so that they can be put into a
  // string literal.
  var escapes = {
    "'": "'",
    '\\': '\\',
    '\r': 'r',
    '\n': 'n',
    '\u2028': 'u2028',
    '\u2029': 'u2029'
  };

  var escapeRegExp = /\\|'|\r|\n|\u2028|\u2029/g;

  var escapeChar = function(match) {
    return '\\' + escapes[match];
  };

  // JavaScript micro-templating, similar to John Resig's implementation.
  // Underscore templating handles arbitrary delimiters, preserves whitespace,
  // and correctly escapes quotes within interpolated code.
  // NB: `oldSettings` only exists for backwards compatibility.
  _.template = function(text, settings, oldSettings) {
    if (!settings && oldSettings) settings = oldSettings;
    settings = _.defaults({}, settings, _.templateSettings);

    // Combine delimiters into one regular expression via alternation.
    var matcher = RegExp([
      (settings.escape || noMatch).source,
      (settings.interpolate || noMatch).source,
      (settings.evaluate || noMatch).source
    ].join('|') + '|$', 'g');

    // Compile the template source, escaping string literals appropriately.
    var index = 0;
    var source = "__p+='";
    text.replace(matcher, function(match, escape, interpolate, evaluate, offset) {
      source += text.slice(index, offset).replace(escapeRegExp, escapeChar);
      index = offset + match.length;

      if (escape) {
        source += "'+\n((__t=(" + escape + "))==null?'':_.escape(__t))+\n'";
      } else if (interpolate) {
        source += "'+\n((__t=(" + interpolate + "))==null?'':__t)+\n'";
      } else if (evaluate) {
        source += "';\n" + evaluate + "\n__p+='";
      }

      // Adobe VMs need the match returned to produce the correct offest.
      return match;
    });
    source += "';\n";

    // If a variable is not specified, place data values in local scope.
    if (!settings.variable) source = 'with(obj||{}){\n' + source + '}\n';

    source = "var __t,__p='',__j=Array.prototype.join," +
      "print=function(){__p+=__j.call(arguments,'');};\n" +
      source + 'return __p;\n';

    var render;
    try {
      render = new Function(settings.variable || 'obj', '_', source);
    } catch (e) {
      e.source = source;
      throw e;
    }

    var template = function(data) {
      return render.call(this, data, _);
    };

    // Provide the compiled source as a convenience for precompilation.
    var argument = settings.variable || 'obj';
    template.source = 'function(' + argument + '){\n' + source + '}';

    return template;
  };

  // Add a "chain" function. Start chaining a wrapped Underscore object.
  _.chain = function(obj) {
    var instance = _(obj);
    instance._chain = true;
    return instance;
  };

  // OOP
  // ---------------
  // If Underscore is called as a function, it returns a wrapped object that
  // can be used OO-style. This wrapper holds altered versions of all the
  // underscore functions. Wrapped objects may be chained.

  // Helper function to continue chaining intermediate results.
  var chainResult = function(instance, obj) {
    return instance._chain ? _(obj).chain() : obj;
  };

  // Add your own custom functions to the Underscore object.
  _.mixin = function(obj) {
    _.each(_.functions(obj), function(name) {
      var func = _[name] = obj[name];
      _.prototype[name] = function() {
        var args = [this._wrapped];
        push.apply(args, arguments);
        return chainResult(this, func.apply(_, args));
      };
    });
  };

  // Add all of the Underscore functions to the wrapper object.
  _.mixin(_);

  // Add all mutator Array functions to the wrapper.
  _.each(['pop', 'push', 'reverse', 'shift', 'sort', 'splice', 'unshift'], function(name) {
    var method = ArrayProto[name];
    _.prototype[name] = function() {
      var obj = this._wrapped;
      method.apply(obj, arguments);
      if ((name === 'shift' || name === 'splice') && obj.length === 0) delete obj[0];
      return chainResult(this, obj);
    };
  });

  // Add all accessor Array functions to the wrapper.
  _.each(['concat', 'join', 'slice'], function(name) {
    var method = ArrayProto[name];
    _.prototype[name] = function() {
      return chainResult(this, method.apply(this._wrapped, arguments));
    };
  });

  // Extracts the result from a wrapped and chained object.
  _.prototype.value = function() {
    return this._wrapped;
  };

  // Provide unwrapping proxy for some methods used in engine operations
  // such as arithmetic and JSON stringification.
  _.prototype.valueOf = _.prototype.toJSON = _.prototype.value;

  _.prototype.toString = function() {
    return '' + this._wrapped;
  };

  // AMD registration happens at the end for compatibility with AMD loaders
  // that may not enforce next-turn semantics on modules. Even though general
  // practice for AMD registration is to be anonymous, underscore registers
  // as a named module because, like jQuery, it is a base library that is
  // popular enough to be bundled in a third party lib, but not be part of
  // an AMD load request. Those cases could generate an error when an
  // anonymous define() is called outside of a loader request.
  if (typeof define === 'function' && define.amd) {
    define('underscore', [], function() {
      return _;
    });
  }
}());

;(function () {
	'use strict';

	/**
	 * @preserve FastClick: polyfill to remove click delays on browsers with touch UIs.
	 *
	 * @codingstandard ftlabs-jsv2
	 * @copyright The Financial Times Limited [All Rights Reserved]
	 * @license MIT License (see LICENSE.txt)
	 */

	/*jslint browser:true, node:true*/
	/*global define, Event, Node*/


	/**
	 * Instantiate fast-clicking listeners on the specified layer.
	 *
	 * @constructor
	 * @param {Element} layer The layer to listen on
	 * @param {Object} [options={}] The options to override the defaults
	 */
	function FastClick(layer, options) {
		var oldOnClick;

		options = options || {};

		/**
		 * Whether a click is currently being tracked.
		 *
		 * @type boolean
		 */
		this.trackingClick = false;


		/**
		 * Timestamp for when click tracking started.
		 *
		 * @type number
		 */
		this.trackingClickStart = 0;


		/**
		 * The element being tracked for a click.
		 *
		 * @type EventTarget
		 */
		this.targetElement = null;


		/**
		 * X-coordinate of touch start event.
		 *
		 * @type number
		 */
		this.touchStartX = 0;


		/**
		 * Y-coordinate of touch start event.
		 *
		 * @type number
		 */
		this.touchStartY = 0;


		/**
		 * ID of the last touch, retrieved from Touch.identifier.
		 *
		 * @type number
		 */
		this.lastTouchIdentifier = 0;


		/**
		 * Touchmove boundary, beyond which a click will be cancelled.
		 *
		 * @type number
		 */
		this.touchBoundary = options.touchBoundary || 10;


		/**
		 * The FastClick layer.
		 *
		 * @type Element
		 */
		this.layer = layer;

		/**
		 * The minimum time between tap(touchstart and touchend) events
		 *
		 * @type number
		 */
		this.tapDelay = options.tapDelay || 200;

		/**
		 * The maximum time for a tap
		 *
		 * @type number
		 */
		this.tapTimeout = options.tapTimeout || 700;

		if (FastClick.notNeeded(layer)) {
			return;
		}

		// Some old versions of Android don't have Function.prototype.bind
		function bind(method, context) {
			return function() { return method.apply(context, arguments); };
		}


		var methods = ['onMouse', 'onClick', 'onTouchStart', 'onTouchMove', 'onTouchEnd', 'onTouchCancel'];
		var context = this;
		for (var i = 0, l = methods.length; i < l; i++) {
			context[methods[i]] = bind(context[methods[i]], context);
		}

		// Set up event handlers as required
		if (deviceIsAndroid) {
			layer.addEventListener('mouseover', this.onMouse, true);
			layer.addEventListener('mousedown', this.onMouse, true);
			layer.addEventListener('mouseup', this.onMouse, true);
		}

		layer.addEventListener('click', this.onClick, true);
		layer.addEventListener('touchstart', this.onTouchStart, false);
		layer.addEventListener('touchmove', this.onTouchMove, false);
		layer.addEventListener('touchend', this.onTouchEnd, false);
		layer.addEventListener('touchcancel', this.onTouchCancel, false);

		// Hack is required for browsers that don't support Event#stopImmediatePropagation (e.g. Android 2)
		// which is how FastClick normally stops click events bubbling to callbacks registered on the FastClick
		// layer when they are cancelled.
		if (!Event.prototype.stopImmediatePropagation) {
			layer.removeEventListener = function(type, callback, capture) {
				var rmv = Node.prototype.removeEventListener;
				if (type === 'click') {
					rmv.call(layer, type, callback.hijacked || callback, capture);
				} else {
					rmv.call(layer, type, callback, capture);
				}
			};

			layer.addEventListener = function(type, callback, capture) {
				var adv = Node.prototype.addEventListener;
				if (type === 'click') {
					adv.call(layer, type, callback.hijacked || (callback.hijacked = function(event) {
						if (!event.propagationStopped) {
							callback(event);
						}
					}), capture);
				} else {
					adv.call(layer, type, callback, capture);
				}
			};
		}

		// If a handler is already declared in the element's onclick attribute, it will be fired before
		// FastClick's onClick handler. Fix this by pulling out the user-defined handler function and
		// adding it as listener.
		if (typeof layer.onclick === 'function') {

			// Android browser on at least 3.2 requires a new reference to the function in layer.onclick
			// - the old one won't work if passed to addEventListener directly.
			oldOnClick = layer.onclick;
			layer.addEventListener('click', function(event) {
				oldOnClick(event);
			}, false);
			layer.onclick = null;
		}
	}

	/**
	* Windows Phone 8.1 fakes user agent string to look like Android and iPhone.
	*
	* @type boolean
	*/
	var deviceIsWindowsPhone = navigator.userAgent.indexOf("Windows Phone") >= 0;

	/**
	 * Android requires exceptions.
	 *
	 * @type boolean
	 */
	var deviceIsAndroid = navigator.userAgent.indexOf('Android') > 0 && !deviceIsWindowsPhone;


	/**
	 * iOS requires exceptions.
	 *
	 * @type boolean
	 */
	var deviceIsIOS = /iP(ad|hone|od)/.test(navigator.userAgent) && !deviceIsWindowsPhone;


	/**
	 * iOS 4 requires an exception for select elements.
	 *
	 * @type boolean
	 */
	var deviceIsIOS4 = deviceIsIOS && (/OS 4_\d(_\d)?/).test(navigator.userAgent);


	/**
	 * iOS 6.0-7.* requires the target element to be manually derived
	 *
	 * @type boolean
	 */
	var deviceIsIOSWithBadTarget = deviceIsIOS && (/OS [6-7]_\d/).test(navigator.userAgent);

	/**
	 * BlackBerry requires exceptions.
	 *
	 * @type boolean
	 */
	var deviceIsBlackBerry10 = navigator.userAgent.indexOf('BB10') > 0;

	/**
	 * Determine whether a given element requires a native click.
	 *
	 * @param {EventTarget|Element} target Target DOM element
	 * @returns {boolean} Returns true if the element needs a native click
	 */
	FastClick.prototype.needsClick = function(target) {
		switch (target.nodeName.toLowerCase()) {

		// Don't send a synthetic click to disabled inputs (issue #62)
		case 'button':
		case 'select':
		case 'textarea':
			if (target.disabled) {
				return true;
			}

			break;
		case 'input':

			// File inputs need real clicks on iOS 6 due to a browser bug (issue #68)
			if ((deviceIsIOS && target.type === 'file') || target.disabled) {
				return true;
			}

			break;
		case 'label':
		case 'iframe': // iOS8 homescreen apps can prevent events bubbling into frames
		case 'video':
			return true;
		}

		return (/\bneedsclick\b/).test(target.className);
	};


	/**
	 * Determine whether a given element requires a call to focus to simulate click into element.
	 *
	 * @param {EventTarget|Element} target Target DOM element
	 * @returns {boolean} Returns true if the element requires a call to focus to simulate native click.
	 */
	FastClick.prototype.needsFocus = function(target) {
		switch (target.nodeName.toLowerCase()) {
		case 'textarea':
			return true;
		case 'select':
			return !deviceIsAndroid;
		case 'input':
			switch (target.type) {
			case 'button':
			case 'checkbox':
			case 'file':
			case 'image':
			case 'radio':
			case 'submit':
				return false;
			}

			// No point in attempting to focus disabled inputs
			return !target.disabled && !target.readOnly;
		default:
			return (/\bneedsfocus\b/).test(target.className);
		}
	};


	/**
	 * Send a click event to the specified element.
	 *
	 * @param {EventTarget|Element} targetElement
	 * @param {Event} event
	 */
	FastClick.prototype.sendClick = function(targetElement, event) {
		var clickEvent, touch;

		// On some Android devices activeElement needs to be blurred otherwise the synthetic click will have no effect (#24)
		if (document.activeElement && document.activeElement !== targetElement) {
			document.activeElement.blur();
		}

		touch = event.changedTouches[0];

		// Synthesise a click event, with an extra attribute so it can be tracked
		clickEvent = document.createEvent('MouseEvents');
		clickEvent.initMouseEvent(this.determineEventType(targetElement), true, true, window, 1, touch.screenX, touch.screenY, touch.clientX, touch.clientY, false, false, false, false, 0, null);
		clickEvent.forwardedTouchEvent = true;
		targetElement.dispatchEvent(clickEvent);
	};

	FastClick.prototype.determineEventType = function(targetElement) {

		//Issue #159: Android Chrome Select Box does not open with a synthetic click event
		if (deviceIsAndroid && targetElement.tagName.toLowerCase() === 'select') {
			return 'mousedown';
		}

		return 'click';
	};


	/**
	 * @param {EventTarget|Element} targetElement
	 */
	FastClick.prototype.focus = function(targetElement) {
		var length;

		// Issue #160: on iOS 7, some input elements (e.g. date datetime month) throw a vague TypeError on setSelectionRange. These elements don't have an integer value for the selectionStart and selectionEnd properties, but unfortunately that can't be used for detection because accessing the properties also throws a TypeError. Just check the type instead. Filed as Apple bug #15122724.
		if (deviceIsIOS && targetElement.setSelectionRange && targetElement.type.indexOf('date') !== 0 && targetElement.type !== 'time' && targetElement.type !== 'month') {
			length = targetElement.value.length;
			targetElement.setSelectionRange(length, length);
		} else {
			targetElement.focus();
		}
	};


	/**
	 * Check whether the given target element is a child of a scrollable layer and if so, set a flag on it.
	 *
	 * @param {EventTarget|Element} targetElement
	 */
	FastClick.prototype.updateScrollParent = function(targetElement) {
		var scrollParent, parentElement;

		scrollParent = targetElement.fastClickScrollParent;

		// Attempt to discover whether the target element is contained within a scrollable layer. Re-check if the
		// target element was moved to another parent.
		if (!scrollParent || !scrollParent.contains(targetElement)) {
			parentElement = targetElement;
			do {
				if (parentElement.scrollHeight > parentElement.offsetHeight) {
					scrollParent = parentElement;
					targetElement.fastClickScrollParent = parentElement;
					break;
				}

				parentElement = parentElement.parentElement;
			} while (parentElement);
		}

		// Always update the scroll top tracker if possible.
		if (scrollParent) {
			scrollParent.fastClickLastScrollTop = scrollParent.scrollTop;
		}
	};


	/**
	 * @param {EventTarget} targetElement
	 * @returns {Element|EventTarget}
	 */
	FastClick.prototype.getTargetElementFromEventTarget = function(eventTarget) {

		// On some older browsers (notably Safari on iOS 4.1 - see issue #56) the event target may be a text node.
		if (eventTarget.nodeType === Node.TEXT_NODE) {
			return eventTarget.parentNode;
		}

		return eventTarget;
	};


	/**
	 * On touch start, record the position and scroll offset.
	 *
	 * @param {Event} event
	 * @returns {boolean}
	 */
	FastClick.prototype.onTouchStart = function(event) {
		var targetElement, touch, selection;

		// Ignore multiple touches, otherwise pinch-to-zoom is prevented if both fingers are on the FastClick element (issue #111).
		if (event.targetTouches.length > 1) {
			return true;
		}

		targetElement = this.getTargetElementFromEventTarget(event.target);
		touch = event.targetTouches[0];

		if (deviceIsIOS) {

			// Only trusted events will deselect text on iOS (issue #49)
			selection = window.getSelection();
			if (selection.rangeCount && !selection.isCollapsed) {
				return true;
			}

			if (!deviceIsIOS4) {

				// Weird things happen on iOS when an alert or confirm dialog is opened from a click event callback (issue #23):
				// when the user next taps anywhere else on the page, new touchstart and touchend events are dispatched
				// with the same identifier as the touch event that previously triggered the click that triggered the alert.
				// Sadly, there is an issue on iOS 4 that causes some normal touch events to have the same identifier as an
				// immediately preceeding touch event (issue #52), so this fix is unavailable on that platform.
				// Issue 120: touch.identifier is 0 when Chrome dev tools 'Emulate touch events' is set with an iOS device UA string,
				// which causes all touch events to be ignored. As this block only applies to iOS, and iOS identifiers are always long,
				// random integers, it's safe to to continue if the identifier is 0 here.
				if (touch.identifier && touch.identifier === this.lastTouchIdentifier) {
					event.preventDefault();
					return false;
				}

				this.lastTouchIdentifier = touch.identifier;

				// If the target element is a child of a scrollable layer (using -webkit-overflow-scrolling: touch) and:
				// 1) the user does a fling scroll on the scrollable layer
				// 2) the user stops the fling scroll with another tap
				// then the event.target of the last 'touchend' event will be the element that was under the user's finger
				// when the fling scroll was started, causing FastClick to send a click event to that layer - unless a check
				// is made to ensure that a parent layer was not scrolled before sending a synthetic click (issue #42).
				this.updateScrollParent(targetElement);
			}
		}

		this.trackingClick = true;
		this.trackingClickStart = event.timeStamp;
		this.targetElement = targetElement;

		this.touchStartX = touch.pageX;
		this.touchStartY = touch.pageY;

		// Prevent phantom clicks on fast double-tap (issue #36)
		if ((event.timeStamp - this.lastClickTime) < this.tapDelay) {
			event.preventDefault();
		}

		return true;
	};


	/**
	 * Based on a touchmove event object, check whether the touch has moved past a boundary since it started.
	 *
	 * @param {Event} event
	 * @returns {boolean}
	 */
	FastClick.prototype.touchHasMoved = function(event) {
		var touch = event.changedTouches[0], boundary = this.touchBoundary;

		if (Math.abs(touch.pageX - this.touchStartX) > boundary || Math.abs(touch.pageY - this.touchStartY) > boundary) {
			return true;
		}

		return false;
	};


	/**
	 * Update the last position.
	 *
	 * @param {Event} event
	 * @returns {boolean}
	 */
	FastClick.prototype.onTouchMove = function(event) {
		if (!this.trackingClick) {
			return true;
		}

		// If the touch has moved, cancel the click tracking
		if (this.targetElement !== this.getTargetElementFromEventTarget(event.target) || this.touchHasMoved(event)) {
			this.trackingClick = false;
			this.targetElement = null;
		}

		return true;
	};


	/**
	 * Attempt to find the labelled control for the given label element.
	 *
	 * @param {EventTarget|HTMLLabelElement} labelElement
	 * @returns {Element|null}
	 */
	FastClick.prototype.findControl = function(labelElement) {

		// Fast path for newer browsers supporting the HTML5 control attribute
		if (labelElement.control !== undefined) {
			return labelElement.control;
		}

		// All browsers under test that support touch events also support the HTML5 htmlFor attribute
		if (labelElement.htmlFor) {
			return document.getElementById(labelElement.htmlFor);
		}

		// If no for attribute exists, attempt to retrieve the first labellable descendant element
		// the list of which is defined here: http://www.w3.org/TR/html5/forms.html#category-label
		return labelElement.querySelector('button, input:not([type=hidden]), keygen, meter, output, progress, select, textarea');
	};


	/**
	 * On touch end, determine whether to send a click event at once.
	 *
	 * @param {Event} event
	 * @returns {boolean}
	 */
	FastClick.prototype.onTouchEnd = function(event) {
		var forElement, trackingClickStart, targetTagName, scrollParent, touch, targetElement = this.targetElement;

		if (!this.trackingClick) {
			return true;
		}

		// Prevent phantom clicks on fast double-tap (issue #36)
		if ((event.timeStamp - this.lastClickTime) < this.tapDelay) {
			this.cancelNextClick = true;
			return true;
		}

		if ((event.timeStamp - this.trackingClickStart) > this.tapTimeout) {
			return true;
		}

		// Reset to prevent wrong click cancel on input (issue #156).
		this.cancelNextClick = false;

		this.lastClickTime = event.timeStamp;

		trackingClickStart = this.trackingClickStart;
		this.trackingClick = false;
		this.trackingClickStart = 0;

		// On some iOS devices, the targetElement supplied with the event is invalid if the layer
		// is performing a transition or scroll, and has to be re-detected manually. Note that
		// for this to function correctly, it must be called *after* the event target is checked!
		// See issue #57; also filed as rdar://13048589 .
		if (deviceIsIOSWithBadTarget) {
			touch = event.changedTouches[0];

			// In certain cases arguments of elementFromPoint can be negative, so prevent setting targetElement to null
			targetElement = document.elementFromPoint(touch.pageX - window.pageXOffset, touch.pageY - window.pageYOffset) || targetElement;
			targetElement.fastClickScrollParent = this.targetElement.fastClickScrollParent;
		}

		targetTagName = targetElement.tagName.toLowerCase();
		if (targetTagName === 'label') {
			forElement = this.findControl(targetElement);
			if (forElement) {
				this.focus(targetElement);
				if (deviceIsAndroid) {
					return false;
				}

				targetElement = forElement;
			}
		} else if (this.needsFocus(targetElement)) {

			// Case 1: If the touch started a while ago (best guess is 100ms based on tests for issue #36) then focus will be triggered anyway. Return early and unset the target element reference so that the subsequent click will be allowed through.
			// Case 2: Without this exception for input elements tapped when the document is contained in an iframe, then any inputted text won't be visible even though the value attribute is updated as the user types (issue #37).
			if ((event.timeStamp - trackingClickStart) > 100 || (deviceIsIOS && window.top !== window && targetTagName === 'input')) {
				this.targetElement = null;
				return false;
			}

			this.focus(targetElement);
			this.sendClick(targetElement, event);

			// Select elements need the event to go through on iOS 4, otherwise the selector menu won't open.
			// Also this breaks opening selects when VoiceOver is active on iOS6, iOS7 (and possibly others)
			if (!deviceIsIOS || targetTagName !== 'select') {
				this.targetElement = null;
				event.preventDefault();
			}

			return false;
		}

		if (deviceIsIOS && !deviceIsIOS4) {

			// Don't send a synthetic click event if the target element is contained within a parent layer that was scrolled
			// and this tap is being used to stop the scrolling (usually initiated by a fling - issue #42).
			scrollParent = targetElement.fastClickScrollParent;
			if (scrollParent && scrollParent.fastClickLastScrollTop !== scrollParent.scrollTop) {
				return true;
			}
		}

		// Prevent the actual click from going though - unless the target node is marked as requiring
		// real clicks or if it is in the whitelist in which case only non-programmatic clicks are permitted.
		if (!this.needsClick(targetElement)) {
			event.preventDefault();
			this.sendClick(targetElement, event);
		}

		return false;
	};


	/**
	 * On touch cancel, stop tracking the click.
	 *
	 * @returns {void}
	 */
	FastClick.prototype.onTouchCancel = function() {
		this.trackingClick = false;
		this.targetElement = null;
	};


	/**
	 * Determine mouse events which should be permitted.
	 *
	 * @param {Event} event
	 * @returns {boolean}
	 */
	FastClick.prototype.onMouse = function(event) {

		// If a target element was never set (because a touch event was never fired) allow the event
		if (!this.targetElement) {
			return true;
		}

		if (event.forwardedTouchEvent) {
			return true;
		}

		// Programmatically generated events targeting a specific element should be permitted
		if (!event.cancelable) {
			return true;
		}

		// Derive and check the target element to see whether the mouse event needs to be permitted;
		// unless explicitly enabled, prevent non-touch click events from triggering actions,
		// to prevent ghost/doubleclicks.
		if (!this.needsClick(this.targetElement) || this.cancelNextClick) {

			// Prevent any user-added listeners declared on FastClick element from being fired.
			if (event.stopImmediatePropagation) {
				event.stopImmediatePropagation();
			} else {

				// Part of the hack for browsers that don't support Event#stopImmediatePropagation (e.g. Android 2)
				event.propagationStopped = true;
			}

			// Cancel the event
			event.stopPropagation();
			event.preventDefault();

			return false;
		}

		// If the mouse event is permitted, return true for the action to go through.
		return true;
	};


	/**
	 * On actual clicks, determine whether this is a touch-generated click, a click action occurring
	 * naturally after a delay after a touch (which needs to be cancelled to avoid duplication), or
	 * an actual click which should be permitted.
	 *
	 * @param {Event} event
	 * @returns {boolean}
	 */
	FastClick.prototype.onClick = function(event) {
		var permitted;

		// It's possible for another FastClick-like library delivered with third-party code to fire a click event before FastClick does (issue #44). In that case, set the click-tracking flag back to false and return early. This will cause onTouchEnd to return early.
		if (this.trackingClick) {
			this.targetElement = null;
			this.trackingClick = false;
			return true;
		}

		// Very odd behaviour on iOS (issue #18): if a submit element is present inside a form and the user hits enter in the iOS simulator or clicks the Go button on the pop-up OS keyboard the a kind of 'fake' click event will be triggered with the submit-type input element as the target.
		if (event.target.type === 'submit' && event.detail === 0) {
			return true;
		}

		permitted = this.onMouse(event);

		// Only unset targetElement if the click is not permitted. This will ensure that the check for !targetElement in onMouse fails and the browser's click doesn't go through.
		if (!permitted) {
			this.targetElement = null;
		}

		// If clicks are permitted, return true for the action to go through.
		return permitted;
	};


	/**
	 * Remove all FastClick's event listeners.
	 *
	 * @returns {void}
	 */
	FastClick.prototype.destroy = function() {
		var layer = this.layer;

		if (deviceIsAndroid) {
			layer.removeEventListener('mouseover', this.onMouse, true);
			layer.removeEventListener('mousedown', this.onMouse, true);
			layer.removeEventListener('mouseup', this.onMouse, true);
		}

		layer.removeEventListener('click', this.onClick, true);
		layer.removeEventListener('touchstart', this.onTouchStart, false);
		layer.removeEventListener('touchmove', this.onTouchMove, false);
		layer.removeEventListener('touchend', this.onTouchEnd, false);
		layer.removeEventListener('touchcancel', this.onTouchCancel, false);
	};


	/**
	 * Check whether FastClick is needed.
	 *
	 * @param {Element} layer The layer to listen on
	 */
	FastClick.notNeeded = function(layer) {
		var metaViewport;
		var chromeVersion;
		var blackberryVersion;
		var firefoxVersion;

		// Devices that don't support touch don't need FastClick
		if (typeof window.ontouchstart === 'undefined') {
			return true;
		}

		// Chrome version - zero for other browsers
		chromeVersion = +(/Chrome\/([0-9]+)/.exec(navigator.userAgent) || [,0])[1];

		if (chromeVersion) {

			if (deviceIsAndroid) {
				metaViewport = document.querySelector('meta[name=viewport]');

				if (metaViewport) {
					// Chrome on Android with user-scalable="no" doesn't need FastClick (issue #89)
					if (metaViewport.content.indexOf('user-scalable=no') !== -1) {
						return true;
					}
					// Chrome 32 and above with width=device-width or less don't need FastClick
					if (chromeVersion > 31 && document.documentElement.scrollWidth <= window.outerWidth) {
						return true;
					}
				}

			// Chrome desktop doesn't need FastClick (issue #15)
			} else {
				return true;
			}
		}

		if (deviceIsBlackBerry10) {
			blackberryVersion = navigator.userAgent.match(/Version\/([0-9]*)\.([0-9]*)/);

			// BlackBerry 10.3+ does not require Fastclick library.
			// https://github.com/ftlabs/fastclick/issues/251
			if (blackberryVersion[1] >= 10 && blackberryVersion[2] >= 3) {
				metaViewport = document.querySelector('meta[name=viewport]');

				if (metaViewport) {
					// user-scalable=no eliminates click delay.
					if (metaViewport.content.indexOf('user-scalable=no') !== -1) {
						return true;
					}
					// width=device-width (or less than device-width) eliminates click delay.
					if (document.documentElement.scrollWidth <= window.outerWidth) {
						return true;
					}
				}
			}
		}

		// IE10 with -ms-touch-action: none or manipulation, which disables double-tap-to-zoom (issue #97)
		if (layer.style.msTouchAction === 'none' || layer.style.touchAction === 'manipulation') {
			return true;
		}

		// Firefox version - zero for other browsers
		firefoxVersion = +(/Firefox\/([0-9]+)/.exec(navigator.userAgent) || [,0])[1];

		if (firefoxVersion >= 27) {
			// Firefox 27+ does not have tap delay if the content is not zoomable - https://bugzilla.mozilla.org/show_bug.cgi?id=922896

			metaViewport = document.querySelector('meta[name=viewport]');
			if (metaViewport && (metaViewport.content.indexOf('user-scalable=no') !== -1 || document.documentElement.scrollWidth <= window.outerWidth)) {
				return true;
			}
		}

		// IE11: prefixed -ms-touch-action is no longer supported and it's recomended to use non-prefixed version
		// http://msdn.microsoft.com/en-us/library/windows/apps/Hh767313.aspx
		if (layer.style.touchAction === 'none' || layer.style.touchAction === 'manipulation') {
			return true;
		}

		return false;
	};


	/**
	 * Factory method for creating a FastClick object
	 *
	 * @param {Element} layer The layer to listen on
	 * @param {Object} [options={}] The options to override the defaults
	 */
	FastClick.attach = function(layer, options) {
		return new FastClick(layer, options);
	};


	if (typeof define === 'function' && typeof define.amd === 'object' && define.amd) {

		// AMD. Register as an anonymous module.
		define(function() {
			return FastClick;
		});
	} else if (typeof module !== 'undefined' && module.exports) {
		module.exports = FastClick.attach;
		module.exports.FastClick = FastClick;
	} else {
		window.FastClick = FastClick;
	}
}());

/* Modified for sessionStorage support and EDGE bug */
/**
 * Simple localStorage with Cookie Fallback
 * v.1.0.0
 *
 * USAGE:
 * ----------------------------------------
 * Set New / Modify:
 *   store('my_key', 'some_value');
 *
 * Retrieve:
 *   store('my_key');
 *
 * Delete / Remove:
 *   store('my_key', null);
 */

(function() {
    if (typeof exports !== 'undefined') {
        if (typeof module !== 'undefined' && module.exports) {
            exports = module.exports = store;
        }
        exports.store = store;
    } else {
        window.store = store;
    }

    function store(key, value, options) {
        options = options || {};

        var lsSupport = false;
        var storage = options.useSession ? 'sessionStorage' : 'localStorage';

        if (!options.useCookies) {
            // localstorage & sessionstorage falls with error in EDGE for local files
            try {
                lsSupport = Boolean(window[storage]);
            } catch (e) {
                lsSupport = false;
            }
        }

        // If value is detected, set new or modify store
        if (typeof value !== "undefined" && value !== null) {
            // Convert object values to JSON
            if ( typeof value === 'object' ) {
                value = JSON.stringify(value);
            }
            // Set the store
            if (lsSupport) { // Native support
                window[storage].setItem(key, value);
            } else { // Use Cookie
                createCookie(key, value, options.expires || 30);
            }
        }

        // No value supplied, return value
        if (typeof value === "undefined") {
            // Get value
            if (lsSupport) { // Native support
                data = window[storage].getItem(key);
            } else { // Use cookie
                data = readCookie(key);
            }

            // Try to parse JSON...
            try {
                data = JSON.parse(data);
            }
            catch(e) {
                data = data;
            }

            return data;

        }

        // Null specified, remove store
        if (value === null) {
            if (lsSupport) { // Native support
                window[storage].removeItem(key);
            } else { // Use cookie
                createCookie(key, '', -1);
            }
        }

        /**
         * Creates new cookie or removes cookie with negative expiration
         * @param  key       The key or identifier for the store
         * @param  value     Contents of the store
         * @param  exp       Expiration - creation defaults to 30 days
         */

        function createCookie(key, value, exp) {
            var date = new Date();
            date.setTime(date.getTime() + (exp * 24 * 60 * 60 * 1000));
            var expires = "; expires=" + date.toGMTString();
            document.cookie = key + "=" + value + expires + "; path=/";
        }

        /**
         * Returns contents of cookie
         * @param  key       The key or identifier for the store
         */

        function readCookie(key) {
            var nameEQ = key + "=";
            var ca = document.cookie.split(';');
            for (var i = 0, max = ca.length; i < max; i++) {
                var c = ca[i];
                while (c.charAt(0) === ' ') c = c.substring(1, c.length);
                if (c.indexOf(nameEQ) === 0) return c.substring(nameEQ.length, c.length);
            }
            return null;
        }

    };
}());

 

/*!
  Highlight.js v11.9.0 (git: b7ec4bfafc)
  (c) 2006-2024 undefined and other contributors
  License: BSD-3-Clause
 */
var hljs=function(){"use strict";function e(t){
return t instanceof Map?t.clear=t.delete=t.set=()=>{
throw Error("map is read-only")}:t instanceof Set&&(t.add=t.clear=t.delete=()=>{
throw Error("set is read-only")
}),Object.freeze(t),Object.getOwnPropertyNames(t).forEach((n=>{
const i=t[n],s=typeof i;"object"!==s&&"function"!==s||Object.isFrozen(i)||e(i)
})),t}class t{constructor(e){
void 0===e.data&&(e.data={}),this.data=e.data,this.isMatchIgnored=!1}
ignoreMatch(){this.isMatchIgnored=!0}}function n(e){
return e.replace(/&/g,"&amp;").replace(/</g,"&lt;").replace(/>/g,"&gt;").replace(/"/g,"&quot;").replace(/'/g,"&#x27;")
}function i(e,...t){const n=Object.create(null);for(const t in e)n[t]=e[t]
;return t.forEach((e=>{for(const t in e)n[t]=e[t]})),n}const s=e=>!!e.scope
;class o{constructor(e,t){
this.buffer="",this.classPrefix=t.classPrefix,e.walk(this)}addText(e){
this.buffer+=n(e)}openNode(e){if(!s(e))return;const t=((e,{prefix:t})=>{
if(e.startsWith("language:"))return e.replace("language:","language-")
;if(e.includes(".")){const n=e.split(".")
;return[`${t}${n.shift()}`,...n.map(((e,t)=>`${e}${"_".repeat(t+1)}`))].join(" ")
}return`${t}${e}`})(e.scope,{prefix:this.classPrefix});this.span(t)}
closeNode(e){s(e)&&(this.buffer+="</span>")}value(){return this.buffer}span(e){
this.buffer+=`<span class="${e}">`}}const r=(e={})=>{const t={children:[]}
;return Object.assign(t,e),t};class a{constructor(){
this.rootNode=r(),this.stack=[this.rootNode]}get top(){
return this.stack[this.stack.length-1]}get root(){return this.rootNode}add(e){
this.top.children.push(e)}openNode(e){const t=r({scope:e})
;this.add(t),this.stack.push(t)}closeNode(){
if(this.stack.length>1)return this.stack.pop()}closeAllNodes(){
for(;this.closeNode(););}toJSON(){return JSON.stringify(this.rootNode,null,4)}
walk(e){return this.constructor._walk(e,this.rootNode)}static _walk(e,t){
return"string"==typeof t?e.addText(t):t.children&&(e.openNode(t),
t.children.forEach((t=>this._walk(e,t))),e.closeNode(t)),e}static _collapse(e){
"string"!=typeof e&&e.children&&(e.children.every((e=>"string"==typeof e))?e.children=[e.children.join("")]:e.children.forEach((e=>{
a._collapse(e)})))}}class c extends a{constructor(e){super(),this.options=e}
addText(e){""!==e&&this.add(e)}startScope(e){this.openNode(e)}endScope(){
this.closeNode()}__addSublanguage(e,t){const n=e.root
;t&&(n.scope="language:"+t),this.add(n)}toHTML(){
return new o(this,this.options).value()}finalize(){
return this.closeAllNodes(),!0}}function l(e){
return e?"string"==typeof e?e:e.source:null}function g(e){return h("(?=",e,")")}
function u(e){return h("(?:",e,")*")}function d(e){return h("(?:",e,")?")}
function h(...e){return e.map((e=>l(e))).join("")}function f(...e){const t=(e=>{
const t=e[e.length-1]
;return"object"==typeof t&&t.constructor===Object?(e.splice(e.length-1,1),t):{}
})(e);return"("+(t.capture?"":"?:")+e.map((e=>l(e))).join("|")+")"}
function p(e){return RegExp(e.toString()+"|").exec("").length-1}
const b=/\[(?:[^\\\]]|\\.)*\]|\(\??|\\([1-9][0-9]*)|\\./
;function m(e,{joinWith:t}){let n=0;return e.map((e=>{n+=1;const t=n
;let i=l(e),s="";for(;i.length>0;){const e=b.exec(i);if(!e){s+=i;break}
s+=i.substring(0,e.index),
i=i.substring(e.index+e[0].length),"\\"===e[0][0]&&e[1]?s+="\\"+(Number(e[1])+t):(s+=e[0],
"("===e[0]&&n++)}return s})).map((e=>`(${e})`)).join(t)}
const E="[a-zA-Z]\\w*",x="[a-zA-Z_]\\w*",w="\\b\\d+(\\.\\d+)?",y="(-?)(\\b0[xX][a-fA-F0-9]+|(\\b\\d+(\\.\\d*)?|\\.\\d+)([eE][-+]?\\d+)?)",_="\\b(0b[01]+)",O={
begin:"\\\\[\\s\\S]",relevance:0},v={scope:"string",begin:"'",end:"'",
illegal:"\\n",contains:[O]},k={scope:"string",begin:'"',end:'"',illegal:"\\n",
contains:[O]},N=(e,t,n={})=>{const s=i({scope:"comment",begin:e,end:t,
contains:[]},n);s.contains.push({scope:"doctag",
begin:"[ ]*(?=(TODO|FIXME|NOTE|BUG|OPTIMIZE|HACK|XXX):)",
end:/(TODO|FIXME|NOTE|BUG|OPTIMIZE|HACK|XXX):/,excludeBegin:!0,relevance:0})
;const o=f("I","a","is","so","us","to","at","if","in","it","on",/[A-Za-z]+['](d|ve|re|ll|t|s|n)/,/[A-Za-z]+[-][a-z]+/,/[A-Za-z][a-z]{2,}/)
;return s.contains.push({begin:h(/[ ]+/,"(",o,/[.]?[:]?([.][ ]|[ ])/,"){3}")}),s
},S=N("//","$"),M=N("/\\*","\\*/"),R=N("#","$");var j=Object.freeze({
__proto__:null,APOS_STRING_MODE:v,BACKSLASH_ESCAPE:O,BINARY_NUMBER_MODE:{
scope:"number",begin:_,relevance:0},BINARY_NUMBER_RE:_,COMMENT:N,
C_BLOCK_COMMENT_MODE:M,C_LINE_COMMENT_MODE:S,C_NUMBER_MODE:{scope:"number",
begin:y,relevance:0},C_NUMBER_RE:y,END_SAME_AS_BEGIN:e=>Object.assign(e,{
"on:begin":(e,t)=>{t.data._beginMatch=e[1]},"on:end":(e,t)=>{
t.data._beginMatch!==e[1]&&t.ignoreMatch()}}),HASH_COMMENT_MODE:R,IDENT_RE:E,
MATCH_NOTHING_RE:/\b\B/,METHOD_GUARD:{begin:"\\.\\s*"+x,relevance:0},
NUMBER_MODE:{scope:"number",begin:w,relevance:0},NUMBER_RE:w,
PHRASAL_WORDS_MODE:{
begin:/\b(a|an|the|are|I'm|isn't|don't|doesn't|won't|but|just|should|pretty|simply|enough|gonna|going|wtf|so|such|will|you|your|they|like|more)\b/
},QUOTE_STRING_MODE:k,REGEXP_MODE:{scope:"regexp",begin:/\/(?=[^/\n]*\/)/,
end:/\/[gimuy]*/,contains:[O,{begin:/\[/,end:/\]/,relevance:0,contains:[O]}]},
RE_STARTERS_RE:"!|!=|!==|%|%=|&|&&|&=|\\*|\\*=|\\+|\\+=|,|-|-=|/=|/|:|;|<<|<<=|<=|<|===|==|=|>>>=|>>=|>=|>>>|>>|>|\\?|\\[|\\{|\\(|\\^|\\^=|\\||\\|=|\\|\\||~",
SHEBANG:(e={})=>{const t=/^#![ ]*\//
;return e.binary&&(e.begin=h(t,/.*\b/,e.binary,/\b.*/)),i({scope:"meta",begin:t,
end:/$/,relevance:0,"on:begin":(e,t)=>{0!==e.index&&t.ignoreMatch()}},e)},
TITLE_MODE:{scope:"title",begin:E,relevance:0},UNDERSCORE_IDENT_RE:x,
UNDERSCORE_TITLE_MODE:{scope:"title",begin:x,relevance:0}});function A(e,t){
"."===e.input[e.index-1]&&t.ignoreMatch()}function I(e,t){
void 0!==e.className&&(e.scope=e.className,delete e.className)}function T(e,t){
t&&e.beginKeywords&&(e.begin="\\b("+e.beginKeywords.split(" ").join("|")+")(?!\\.)(?=\\b|\\s)",
e.__beforeBegin=A,e.keywords=e.keywords||e.beginKeywords,delete e.beginKeywords,
void 0===e.relevance&&(e.relevance=0))}function L(e,t){
Array.isArray(e.illegal)&&(e.illegal=f(...e.illegal))}function B(e,t){
if(e.match){
if(e.begin||e.end)throw Error("begin & end are not supported with match")
;e.begin=e.match,delete e.match}}function P(e,t){
void 0===e.relevance&&(e.relevance=1)}const D=(e,t)=>{if(!e.beforeMatch)return
;if(e.starts)throw Error("beforeMatch cannot be used with starts")
;const n=Object.assign({},e);Object.keys(e).forEach((t=>{delete e[t]
})),e.keywords=n.keywords,e.begin=h(n.beforeMatch,g(n.begin)),e.starts={
relevance:0,contains:[Object.assign(n,{endsParent:!0})]
},e.relevance=0,delete n.beforeMatch
},H=["of","and","for","in","not","or","if","then","parent","list","value"],C="keyword"
;function $(e,t,n=C){const i=Object.create(null)
;return"string"==typeof e?s(n,e.split(" ")):Array.isArray(e)?s(n,e):Object.keys(e).forEach((n=>{
Object.assign(i,$(e[n],t,n))})),i;function s(e,n){
t&&(n=n.map((e=>e.toLowerCase()))),n.forEach((t=>{const n=t.split("|")
;i[n[0]]=[e,U(n[0],n[1])]}))}}function U(e,t){
return t?Number(t):(e=>H.includes(e.toLowerCase()))(e)?0:1}const z={},W=e=>{
console.error(e)},X=(e,...t)=>{console.log("WARN: "+e,...t)},G=(e,t)=>{
z[`${e}/${t}`]||(console.log(`Deprecated as of ${e}. ${t}`),z[`${e}/${t}`]=!0)
},K=Error();function F(e,t,{key:n}){let i=0;const s=e[n],o={},r={}
;for(let e=1;e<=t.length;e++)r[e+i]=s[e],o[e+i]=!0,i+=p(t[e-1])
;e[n]=r,e[n]._emit=o,e[n]._multi=!0}function Z(e){(e=>{
e.scope&&"object"==typeof e.scope&&null!==e.scope&&(e.beginScope=e.scope,
delete e.scope)})(e),"string"==typeof e.beginScope&&(e.beginScope={
_wrap:e.beginScope}),"string"==typeof e.endScope&&(e.endScope={_wrap:e.endScope
}),(e=>{if(Array.isArray(e.begin)){
if(e.skip||e.excludeBegin||e.returnBegin)throw W("skip, excludeBegin, returnBegin not compatible with beginScope: {}"),
K
;if("object"!=typeof e.beginScope||null===e.beginScope)throw W("beginScope must be object"),
K;F(e,e.begin,{key:"beginScope"}),e.begin=m(e.begin,{joinWith:""})}})(e),(e=>{
if(Array.isArray(e.end)){
if(e.skip||e.excludeEnd||e.returnEnd)throw W("skip, excludeEnd, returnEnd not compatible with endScope: {}"),
K
;if("object"!=typeof e.endScope||null===e.endScope)throw W("endScope must be object"),
K;F(e,e.end,{key:"endScope"}),e.end=m(e.end,{joinWith:""})}})(e)}function V(e){
function t(t,n){
return RegExp(l(t),"m"+(e.case_insensitive?"i":"")+(e.unicodeRegex?"u":"")+(n?"g":""))
}class n{constructor(){
this.matchIndexes={},this.regexes=[],this.matchAt=1,this.position=0}
addRule(e,t){
t.position=this.position++,this.matchIndexes[this.matchAt]=t,this.regexes.push([t,e]),
this.matchAt+=p(e)+1}compile(){0===this.regexes.length&&(this.exec=()=>null)
;const e=this.regexes.map((e=>e[1]));this.matcherRe=t(m(e,{joinWith:"|"
}),!0),this.lastIndex=0}exec(e){this.matcherRe.lastIndex=this.lastIndex
;const t=this.matcherRe.exec(e);if(!t)return null
;const n=t.findIndex(((e,t)=>t>0&&void 0!==e)),i=this.matchIndexes[n]
;return t.splice(0,n),Object.assign(t,i)}}class s{constructor(){
this.rules=[],this.multiRegexes=[],
this.count=0,this.lastIndex=0,this.regexIndex=0}getMatcher(e){
if(this.multiRegexes[e])return this.multiRegexes[e];const t=new n
;return this.rules.slice(e).forEach((([e,n])=>t.addRule(e,n))),
t.compile(),this.multiRegexes[e]=t,t}resumingScanAtSamePosition(){
return 0!==this.regexIndex}considerAll(){this.regexIndex=0}addRule(e,t){
this.rules.push([e,t]),"begin"===t.type&&this.count++}exec(e){
const t=this.getMatcher(this.regexIndex);t.lastIndex=this.lastIndex
;let n=t.exec(e)
;if(this.resumingScanAtSamePosition())if(n&&n.index===this.lastIndex);else{
const t=this.getMatcher(0);t.lastIndex=this.lastIndex+1,n=t.exec(e)}
return n&&(this.regexIndex+=n.position+1,
this.regexIndex===this.count&&this.considerAll()),n}}
if(e.compilerExtensions||(e.compilerExtensions=[]),
e.contains&&e.contains.includes("self"))throw Error("ERR: contains `self` is not supported at the top-level of a language.  See documentation.")
;return e.classNameAliases=i(e.classNameAliases||{}),function n(o,r){const a=o
;if(o.isCompiled)return a
;[I,B,Z,D].forEach((e=>e(o,r))),e.compilerExtensions.forEach((e=>e(o,r))),
o.__beforeBegin=null,[T,L,P].forEach((e=>e(o,r))),o.isCompiled=!0;let c=null
;return"object"==typeof o.keywords&&o.keywords.$pattern&&(o.keywords=Object.assign({},o.keywords),
c=o.keywords.$pattern,
delete o.keywords.$pattern),c=c||/\w+/,o.keywords&&(o.keywords=$(o.keywords,e.case_insensitive)),
a.keywordPatternRe=t(c,!0),
r&&(o.begin||(o.begin=/\B|\b/),a.beginRe=t(a.begin),o.end||o.endsWithParent||(o.end=/\B|\b/),
o.end&&(a.endRe=t(a.end)),
a.terminatorEnd=l(a.end)||"",o.endsWithParent&&r.terminatorEnd&&(a.terminatorEnd+=(o.end?"|":"")+r.terminatorEnd)),
o.illegal&&(a.illegalRe=t(o.illegal)),
o.contains||(o.contains=[]),o.contains=[].concat(...o.contains.map((e=>(e=>(e.variants&&!e.cachedVariants&&(e.cachedVariants=e.variants.map((t=>i(e,{
variants:null},t)))),e.cachedVariants?e.cachedVariants:q(e)?i(e,{
starts:e.starts?i(e.starts):null
}):Object.isFrozen(e)?i(e):e))("self"===e?o:e)))),o.contains.forEach((e=>{n(e,a)
})),o.starts&&n(o.starts,r),a.matcher=(e=>{const t=new s
;return e.contains.forEach((e=>t.addRule(e.begin,{rule:e,type:"begin"
}))),e.terminatorEnd&&t.addRule(e.terminatorEnd,{type:"end"
}),e.illegal&&t.addRule(e.illegal,{type:"illegal"}),t})(a),a}(e)}function q(e){
return!!e&&(e.endsWithParent||q(e.starts))}class J extends Error{
constructor(e,t){super(e),this.name="HTMLInjectionError",this.html=t}}
const Y=n,Q=i,ee=Symbol("nomatch"),te=n=>{
const i=Object.create(null),s=Object.create(null),o=[];let r=!0
;const a="Could not find the language '{}', did you forget to load/include a language module?",l={
disableAutodetect:!0,name:"Plain text",contains:[]};let p={
ignoreUnescapedHTML:!1,throwUnescapedHTML:!1,noHighlightRe:/^(no-?highlight)$/i,
languageDetectRe:/\blang(?:uage)?-([\w-]+)\b/i,classPrefix:"hljs-",
cssSelector:"pre code",languages:null,__emitter:c};function b(e){
return p.noHighlightRe.test(e)}function m(e,t,n){let i="",s=""
;"object"==typeof t?(i=e,
n=t.ignoreIllegals,s=t.language):(G("10.7.0","highlight(lang, code, ...args) has been deprecated."),
G("10.7.0","Please use highlight(code, options) instead.\nhttps://github.com/highlightjs/highlight.js/issues/2277"),
s=e,i=t),void 0===n&&(n=!0);const o={code:i,language:s};N("before:highlight",o)
;const r=o.result?o.result:E(o.language,o.code,n)
;return r.code=o.code,N("after:highlight",r),r}function E(e,n,s,o){
const c=Object.create(null);function l(){if(!N.keywords)return void M.addText(R)
;let e=0;N.keywordPatternRe.lastIndex=0;let t=N.keywordPatternRe.exec(R),n=""
;for(;t;){n+=R.substring(e,t.index)
;const s=_.case_insensitive?t[0].toLowerCase():t[0],o=(i=s,N.keywords[i]);if(o){
const[e,i]=o
;if(M.addText(n),n="",c[s]=(c[s]||0)+1,c[s]<=7&&(j+=i),e.startsWith("_"))n+=t[0];else{
const n=_.classNameAliases[e]||e;u(t[0],n)}}else n+=t[0]
;e=N.keywordPatternRe.lastIndex,t=N.keywordPatternRe.exec(R)}var i
;n+=R.substring(e),M.addText(n)}function g(){null!=N.subLanguage?(()=>{
if(""===R)return;let e=null;if("string"==typeof N.subLanguage){
if(!i[N.subLanguage])return void M.addText(R)
;e=E(N.subLanguage,R,!0,S[N.subLanguage]),S[N.subLanguage]=e._top
}else e=x(R,N.subLanguage.length?N.subLanguage:null)
;N.relevance>0&&(j+=e.relevance),M.__addSublanguage(e._emitter,e.language)
})():l(),R=""}function u(e,t){
""!==e&&(M.startScope(t),M.addText(e),M.endScope())}function d(e,t){let n=1
;const i=t.length-1;for(;n<=i;){if(!e._emit[n]){n++;continue}
const i=_.classNameAliases[e[n]]||e[n],s=t[n];i?u(s,i):(R=s,l(),R=""),n++}}
function h(e,t){
return e.scope&&"string"==typeof e.scope&&M.openNode(_.classNameAliases[e.scope]||e.scope),
e.beginScope&&(e.beginScope._wrap?(u(R,_.classNameAliases[e.beginScope._wrap]||e.beginScope._wrap),
R=""):e.beginScope._multi&&(d(e.beginScope,t),R="")),N=Object.create(e,{parent:{
value:N}}),N}function f(e,n,i){let s=((e,t)=>{const n=e&&e.exec(t)
;return n&&0===n.index})(e.endRe,i);if(s){if(e["on:end"]){const i=new t(e)
;e["on:end"](n,i),i.isMatchIgnored&&(s=!1)}if(s){
for(;e.endsParent&&e.parent;)e=e.parent;return e}}
if(e.endsWithParent)return f(e.parent,n,i)}function b(e){
return 0===N.matcher.regexIndex?(R+=e[0],1):(T=!0,0)}function m(e){
const t=e[0],i=n.substring(e.index),s=f(N,e,i);if(!s)return ee;const o=N
;N.endScope&&N.endScope._wrap?(g(),
u(t,N.endScope._wrap)):N.endScope&&N.endScope._multi?(g(),
d(N.endScope,e)):o.skip?R+=t:(o.returnEnd||o.excludeEnd||(R+=t),
g(),o.excludeEnd&&(R=t));do{
N.scope&&M.closeNode(),N.skip||N.subLanguage||(j+=N.relevance),N=N.parent
}while(N!==s.parent);return s.starts&&h(s.starts,e),o.returnEnd?0:t.length}
let w={};function y(i,o){const a=o&&o[0];if(R+=i,null==a)return g(),0
;if("begin"===w.type&&"end"===o.type&&w.index===o.index&&""===a){
if(R+=n.slice(o.index,o.index+1),!r){const t=Error(`0 width match regex (${e})`)
;throw t.languageName=e,t.badRule=w.rule,t}return 1}
if(w=o,"begin"===o.type)return(e=>{
const n=e[0],i=e.rule,s=new t(i),o=[i.__beforeBegin,i["on:begin"]]
;for(const t of o)if(t&&(t(e,s),s.isMatchIgnored))return b(n)
;return i.skip?R+=n:(i.excludeBegin&&(R+=n),
g(),i.returnBegin||i.excludeBegin||(R=n)),h(i,e),i.returnBegin?0:n.length})(o)
;if("illegal"===o.type&&!s){
const e=Error('Illegal lexeme "'+a+'" for mode "'+(N.scope||"<unnamed>")+'"')
;throw e.mode=N,e}if("end"===o.type){const e=m(o);if(e!==ee)return e}
if("illegal"===o.type&&""===a)return 1
;if(I>1e5&&I>3*o.index)throw Error("potential infinite loop, way more iterations than matches")
;return R+=a,a.length}const _=O(e)
;if(!_)throw W(a.replace("{}",e)),Error('Unknown language: "'+e+'"')
;const v=V(_);let k="",N=o||v;const S={},M=new p.__emitter(p);(()=>{const e=[]
;for(let t=N;t!==_;t=t.parent)t.scope&&e.unshift(t.scope)
;e.forEach((e=>M.openNode(e)))})();let R="",j=0,A=0,I=0,T=!1;try{
if(_.__emitTokens)_.__emitTokens(n,M);else{for(N.matcher.considerAll();;){
I++,T?T=!1:N.matcher.considerAll(),N.matcher.lastIndex=A
;const e=N.matcher.exec(n);if(!e)break;const t=y(n.substring(A,e.index),e)
;A=e.index+t}y(n.substring(A))}return M.finalize(),k=M.toHTML(),{language:e,
value:k,relevance:j,illegal:!1,_emitter:M,_top:N}}catch(t){
if(t.message&&t.message.includes("Illegal"))return{language:e,value:Y(n),
illegal:!0,relevance:0,_illegalBy:{message:t.message,index:A,
context:n.slice(A-100,A+100),mode:t.mode,resultSoFar:k},_emitter:M};if(r)return{
language:e,value:Y(n),illegal:!1,relevance:0,errorRaised:t,_emitter:M,_top:N}
;throw t}}function x(e,t){t=t||p.languages||Object.keys(i);const n=(e=>{
const t={value:Y(e),illegal:!1,relevance:0,_top:l,_emitter:new p.__emitter(p)}
;return t._emitter.addText(e),t})(e),s=t.filter(O).filter(k).map((t=>E(t,e,!1)))
;s.unshift(n);const o=s.sort(((e,t)=>{
if(e.relevance!==t.relevance)return t.relevance-e.relevance
;if(e.language&&t.language){if(O(e.language).supersetOf===t.language)return 1
;if(O(t.language).supersetOf===e.language)return-1}return 0})),[r,a]=o,c=r
;return c.secondBest=a,c}function w(e){let t=null;const n=(e=>{
let t=e.className+" ";t+=e.parentNode?e.parentNode.className:""
;const n=p.languageDetectRe.exec(t);if(n){const t=O(n[1])
;return t||(X(a.replace("{}",n[1])),
X("Falling back to no-highlight mode for this block.",e)),t?n[1]:"no-highlight"}
return t.split(/\s+/).find((e=>b(e)||O(e)))})(e);if(b(n))return
;if(N("before:highlightElement",{el:e,language:n
}),e.dataset.highlighted)return void console.log("Element previously highlighted. To highlight again, first unset `dataset.highlighted`.",e)
;if(e.children.length>0&&(p.ignoreUnescapedHTML||(console.warn("One of your code blocks includes unescaped HTML. This is a potentially serious security risk."),
console.warn("https://github.com/highlightjs/highlight.js/wiki/security"),
console.warn("The element with unescaped HTML:"),
console.warn(e)),p.throwUnescapedHTML))throw new J("One of your code blocks includes unescaped HTML.",e.innerHTML)
;t=e;const i=t.textContent,o=n?m(i,{language:n,ignoreIllegals:!0}):x(i)
;e.innerHTML=o.value,e.dataset.highlighted="yes",((e,t,n)=>{const i=t&&s[t]||n
;e.classList.add("hljs"),e.classList.add("language-"+i)
})(e,n,o.language),e.result={language:o.language,re:o.relevance,
relevance:o.relevance},o.secondBest&&(e.secondBest={
language:o.secondBest.language,relevance:o.secondBest.relevance
}),N("after:highlightElement",{el:e,result:o,text:i})}let y=!1;function _(){
"loading"!==document.readyState?document.querySelectorAll(p.cssSelector).forEach(w):y=!0
}function O(e){return e=(e||"").toLowerCase(),i[e]||i[s[e]]}
function v(e,{languageName:t}){"string"==typeof e&&(e=[e]),e.forEach((e=>{
s[e.toLowerCase()]=t}))}function k(e){const t=O(e)
;return t&&!t.disableAutodetect}function N(e,t){const n=e;o.forEach((e=>{
e[n]&&e[n](t)}))}
"undefined"!=typeof window&&window.addEventListener&&window.addEventListener("DOMContentLoaded",(()=>{
y&&_()}),!1),Object.assign(n,{highlight:m,highlightAuto:x,highlightAll:_,
highlightElement:w,
highlightBlock:e=>(G("10.7.0","highlightBlock will be removed entirely in v12.0"),
G("10.7.0","Please use highlightElement now."),w(e)),configure:e=>{p=Q(p,e)},
initHighlighting:()=>{
_(),G("10.6.0","initHighlighting() deprecated.  Use highlightAll() now.")},
initHighlightingOnLoad:()=>{
_(),G("10.6.0","initHighlightingOnLoad() deprecated.  Use highlightAll() now.")
},registerLanguage:(e,t)=>{let s=null;try{s=t(n)}catch(t){
if(W("Language definition for '{}' could not be registered.".replace("{}",e)),
!r)throw t;W(t),s=l}
s.name||(s.name=e),i[e]=s,s.rawDefinition=t.bind(null,n),s.aliases&&v(s.aliases,{
languageName:e})},unregisterLanguage:e=>{delete i[e]
;for(const t of Object.keys(s))s[t]===e&&delete s[t]},
listLanguages:()=>Object.keys(i),getLanguage:O,registerAliases:v,
autoDetection:k,inherit:Q,addPlugin:e=>{(e=>{
e["before:highlightBlock"]&&!e["before:highlightElement"]&&(e["before:highlightElement"]=t=>{
e["before:highlightBlock"](Object.assign({block:t.el},t))
}),e["after:highlightBlock"]&&!e["after:highlightElement"]&&(e["after:highlightElement"]=t=>{
e["after:highlightBlock"](Object.assign({block:t.el},t))})})(e),o.push(e)},
removePlugin:e=>{const t=o.indexOf(e);-1!==t&&o.splice(t,1)}}),n.debugMode=()=>{
r=!1},n.safeMode=()=>{r=!0},n.versionString="11.9.0",n.regex={concat:h,
lookahead:g,either:f,optional:d,anyNumberOfTimes:u}
;for(const t in j)"object"==typeof j[t]&&e(j[t]);return Object.assign(n,j),n
},ne=te({});return ne.newInstance=()=>te({}),ne}()
;"object"==typeof exports&&"undefined"!=typeof module&&(module.exports=hljs);/*! `bash` grammar compiled for Highlight.js 11.9.0 */
(()=>{var e=(()=>{"use strict";return e=>{const s=e.regex,t={},n={begin:/\$\{/,
end:/\}/,contains:["self",{begin:/:-/,contains:[t]}]};Object.assign(t,{
className:"variable",variants:[{
begin:s.concat(/\$[\w\d#@][\w\d_]*/,"(?![\\w\\d])(?![$])")},n]});const a={
className:"subst",begin:/\$\(/,end:/\)/,contains:[e.BACKSLASH_ESCAPE]
},i=e.inherit(e.COMMENT(),{match:[/(^|\s)/,/#.*$/],scope:{2:"comment"}}),c={
begin:/<<-?\s*(?=\w+)/,starts:{contains:[e.END_SAME_AS_BEGIN({begin:/(\w+)/,
end:/(\w+)/,className:"string"})]}},o={className:"string",begin:/"/,end:/"/,
contains:[e.BACKSLASH_ESCAPE,t,a]};a.contains.push(o);const r={begin:/\$?\(\(/,
end:/\)\)/,contains:[{begin:/\d+#[0-9a-f]+/,className:"number"},e.NUMBER_MODE,t]
},l=e.SHEBANG({binary:"(fish|bash|zsh|sh|csh|ksh|tcsh|dash|scsh)",relevance:10
}),m={className:"function",begin:/\w[\w\d_]*\s*\(\s*\)\s*\{/,returnBegin:!0,
contains:[e.inherit(e.TITLE_MODE,{begin:/\w[\w\d_]*/})],relevance:0};return{
name:"Bash",aliases:["sh"],keywords:{$pattern:/\b[a-z][a-z0-9._-]+\b/,
keyword:["if","then","else","elif","fi","for","while","until","in","do","done","case","esac","function","select"],
literal:["true","false"],
built_in:["break","cd","continue","eval","exec","exit","export","getopts","hash","pwd","readonly","return","shift","test","times","trap","umask","unset","alias","bind","builtin","caller","command","declare","echo","enable","help","let","local","logout","mapfile","printf","read","readarray","source","type","typeset","ulimit","unalias","set","shopt","autoload","bg","bindkey","bye","cap","chdir","clone","comparguments","compcall","compctl","compdescribe","compfiles","compgroups","compquote","comptags","comptry","compvalues","dirs","disable","disown","echotc","echoti","emulate","fc","fg","float","functions","getcap","getln","history","integer","jobs","kill","limit","log","noglob","popd","print","pushd","pushln","rehash","sched","setcap","setopt","stat","suspend","ttyctl","unfunction","unhash","unlimit","unsetopt","vared","wait","whence","where","which","zcompile","zformat","zftp","zle","zmodload","zparseopts","zprof","zpty","zregexparse","zsocket","zstyle","ztcp","chcon","chgrp","chown","chmod","cp","dd","df","dir","dircolors","ln","ls","mkdir","mkfifo","mknod","mktemp","mv","realpath","rm","rmdir","shred","sync","touch","truncate","vdir","b2sum","base32","base64","cat","cksum","comm","csplit","cut","expand","fmt","fold","head","join","md5sum","nl","numfmt","od","paste","ptx","pr","sha1sum","sha224sum","sha256sum","sha384sum","sha512sum","shuf","sort","split","sum","tac","tail","tr","tsort","unexpand","uniq","wc","arch","basename","chroot","date","dirname","du","echo","env","expr","factor","groups","hostid","id","link","logname","nice","nohup","nproc","pathchk","pinky","printenv","printf","pwd","readlink","runcon","seq","sleep","stat","stdbuf","stty","tee","test","timeout","tty","uname","unlink","uptime","users","who","whoami","yes"]
},contains:[l,e.SHEBANG(),m,r,i,c,{match:/(\/[a-z._-]+)+/},o,{match:/\\"/},{
className:"string",begin:/'/,end:/'/},{match:/\\'/},t]}}})()
;hljs.registerLanguage("bash",e)})();/*! `cmake` grammar compiled for Highlight.js 11.9.0 */
(()=>{var e=(()=>{"use strict";return e=>({name:"CMake",aliases:["cmake.in"],
case_insensitive:!0,keywords:{
keyword:"break cmake_host_system_information cmake_minimum_required cmake_parse_arguments cmake_policy configure_file continue elseif else endforeach endfunction endif endmacro endwhile execute_process file find_file find_library find_package find_path find_program foreach function get_cmake_property get_directory_property get_filename_component get_property if include include_guard list macro mark_as_advanced math message option return separate_arguments set_directory_properties set_property set site_name string unset variable_watch while add_compile_definitions add_compile_options add_custom_command add_custom_target add_definitions add_dependencies add_executable add_library add_link_options add_subdirectory add_test aux_source_directory build_command create_test_sourcelist define_property enable_language enable_testing export fltk_wrap_ui get_source_file_property get_target_property get_test_property include_directories include_external_msproject include_regular_expression install link_directories link_libraries load_cache project qt_wrap_cpp qt_wrap_ui remove_definitions set_source_files_properties set_target_properties set_tests_properties source_group target_compile_definitions target_compile_features target_compile_options target_include_directories target_link_directories target_link_libraries target_link_options target_sources try_compile try_run ctest_build ctest_configure ctest_coverage ctest_empty_binary_directory ctest_memcheck ctest_read_custom_files ctest_run_script ctest_sleep ctest_start ctest_submit ctest_test ctest_update ctest_upload build_name exec_program export_library_dependencies install_files install_programs install_targets load_command make_directory output_required_files remove subdir_depends subdirs use_mangled_mesa utility_source variable_requires write_file qt5_use_modules qt5_use_package qt5_wrap_cpp on off true false and or not command policy target test exists is_newer_than is_directory is_symlink is_absolute matches less greater equal less_equal greater_equal strless strgreater strequal strless_equal strgreater_equal version_less version_greater version_equal version_less_equal version_greater_equal in_list defined"
},contains:[{className:"variable",begin:/\$\{/,end:/\}/
},e.COMMENT(/#\[\[/,/]]/),e.HASH_COMMENT_MODE,e.QUOTE_STRING_MODE,e.NUMBER_MODE]
})})();hljs.registerLanguage("cmake",e)})();/*! `cpp` grammar compiled for Highlight.js 11.9.0 */
(()=>{var e=(()=>{"use strict";return e=>{const t=e.regex,a=e.COMMENT("//","$",{
contains:[{begin:/\\\n/}]
}),n="decltype\\(auto\\)",r="[a-zA-Z_]\\w*::",i="(?!struct)("+n+"|"+t.optional(r)+"[a-zA-Z_]\\w*"+t.optional("<[^<>]+>")+")",s={
className:"type",begin:"\\b[a-z\\d_]*_t\\b"},c={className:"string",variants:[{
begin:'(u8?|U|L)?"',end:'"',illegal:"\\n",contains:[e.BACKSLASH_ESCAPE]},{
begin:"(u8?|U|L)?'(\\\\(x[0-9A-Fa-f]{2}|u[0-9A-Fa-f]{4,8}|[0-7]{3}|\\S)|.)",
end:"'",illegal:"."},e.END_SAME_AS_BEGIN({
begin:/(?:u8?|U|L)?R"([^()\\ ]{0,16})\(/,end:/\)([^()\\ ]{0,16})"/})]},o={
className:"number",variants:[{
begin:"[+-]?(?:(?:[0-9](?:'?[0-9])*\\.(?:[0-9](?:'?[0-9])*)?|\\.[0-9](?:'?[0-9])*)(?:[Ee][+-]?[0-9](?:'?[0-9])*)?|[0-9](?:'?[0-9])*[Ee][+-]?[0-9](?:'?[0-9])*|0[Xx](?:[0-9A-Fa-f](?:'?[0-9A-Fa-f])*(?:\\.(?:[0-9A-Fa-f](?:'?[0-9A-Fa-f])*)?)?|\\.[0-9A-Fa-f](?:'?[0-9A-Fa-f])*)[Pp][+-]?[0-9](?:'?[0-9])*)(?:[Ff](?:16|32|64|128)?|(BF|bf)16|[Ll]|)"
},{
begin:"[+-]?\\b(?:0[Bb][01](?:'?[01])*|0[Xx][0-9A-Fa-f](?:'?[0-9A-Fa-f])*|0(?:'?[0-7])*|[1-9](?:'?[0-9])*)(?:[Uu](?:LL?|ll?)|[Uu][Zz]?|(?:LL?|ll?)[Uu]?|[Zz][Uu]|)"
}],relevance:0},l={className:"meta",begin:/#\s*[a-z]+\b/,end:/$/,keywords:{
keyword:"if else elif endif define undef warning error line pragma _Pragma ifdef ifndef include"
},contains:[{begin:/\\\n/,relevance:0},e.inherit(c,{className:"string"}),{
className:"string",begin:/<.*?>/},a,e.C_BLOCK_COMMENT_MODE]},u={
className:"title",begin:t.optional(r)+e.IDENT_RE,relevance:0
},d=t.optional(r)+e.IDENT_RE+"\\s*\\(",p={
type:["bool","char","char16_t","char32_t","char8_t","double","float","int","long","short","void","wchar_t","unsigned","signed","const","static"],
keyword:["alignas","alignof","and","and_eq","asm","atomic_cancel","atomic_commit","atomic_noexcept","auto","bitand","bitor","break","case","catch","class","co_await","co_return","co_yield","compl","concept","const_cast|10","consteval","constexpr","constinit","continue","decltype","default","delete","do","dynamic_cast|10","else","enum","explicit","export","extern","false","final","for","friend","goto","if","import","inline","module","mutable","namespace","new","noexcept","not","not_eq","nullptr","operator","or","or_eq","override","private","protected","public","reflexpr","register","reinterpret_cast|10","requires","return","sizeof","static_assert","static_cast|10","struct","switch","synchronized","template","this","thread_local","throw","transaction_safe","transaction_safe_dynamic","true","try","typedef","typeid","typename","union","using","virtual","volatile","while","xor","xor_eq"],
literal:["NULL","false","nullopt","nullptr","true"],built_in:["_Pragma"],
_type_hints:["any","auto_ptr","barrier","binary_semaphore","bitset","complex","condition_variable","condition_variable_any","counting_semaphore","deque","false_type","future","imaginary","initializer_list","istringstream","jthread","latch","lock_guard","multimap","multiset","mutex","optional","ostringstream","packaged_task","pair","promise","priority_queue","queue","recursive_mutex","recursive_timed_mutex","scoped_lock","set","shared_future","shared_lock","shared_mutex","shared_timed_mutex","shared_ptr","stack","string_view","stringstream","timed_mutex","thread","true_type","tuple","unique_lock","unique_ptr","unordered_map","unordered_multimap","unordered_multiset","unordered_set","variant","vector","weak_ptr","wstring","wstring_view"]
},_={className:"function.dispatch",relevance:0,keywords:{
_hint:["abort","abs","acos","apply","as_const","asin","atan","atan2","calloc","ceil","cerr","cin","clog","cos","cosh","cout","declval","endl","exchange","exit","exp","fabs","floor","fmod","forward","fprintf","fputs","free","frexp","fscanf","future","invoke","isalnum","isalpha","iscntrl","isdigit","isgraph","islower","isprint","ispunct","isspace","isupper","isxdigit","labs","launder","ldexp","log","log10","make_pair","make_shared","make_shared_for_overwrite","make_tuple","make_unique","malloc","memchr","memcmp","memcpy","memset","modf","move","pow","printf","putchar","puts","realloc","scanf","sin","sinh","snprintf","sprintf","sqrt","sscanf","std","stderr","stdin","stdout","strcat","strchr","strcmp","strcpy","strcspn","strlen","strncat","strncmp","strncpy","strpbrk","strrchr","strspn","strstr","swap","tan","tanh","terminate","to_underlying","tolower","toupper","vfprintf","visit","vprintf","vsprintf"]
},
begin:t.concat(/\b/,/(?!decltype)/,/(?!if)/,/(?!for)/,/(?!switch)/,/(?!while)/,e.IDENT_RE,t.lookahead(/(<[^<>]+>|)\s*\(/))
},m=[_,l,s,a,e.C_BLOCK_COMMENT_MODE,o,c],f={variants:[{begin:/=/,end:/;/},{
begin:/\(/,end:/\)/},{beginKeywords:"new throw return else",end:/;/}],
keywords:p,contains:m.concat([{begin:/\(/,end:/\)/,keywords:p,
contains:m.concat(["self"]),relevance:0}]),relevance:0},g={className:"function",
begin:"("+i+"[\\*&\\s]+)+"+d,returnBegin:!0,end:/[{;=]/,excludeEnd:!0,
keywords:p,illegal:/[^\w\s\*&:<>.]/,contains:[{begin:n,keywords:p,relevance:0},{
begin:d,returnBegin:!0,contains:[u],relevance:0},{begin:/::/,relevance:0},{
begin:/:/,endsWithParent:!0,contains:[c,o]},{relevance:0,match:/,/},{
className:"params",begin:/\(/,end:/\)/,keywords:p,relevance:0,
contains:[a,e.C_BLOCK_COMMENT_MODE,c,o,s,{begin:/\(/,end:/\)/,keywords:p,
relevance:0,contains:["self",a,e.C_BLOCK_COMMENT_MODE,c,o,s]}]
},s,a,e.C_BLOCK_COMMENT_MODE,l]};return{name:"C++",
aliases:["cc","c++","h++","hpp","hh","hxx","cxx"],keywords:p,illegal:"</",
classNameAliases:{"function.dispatch":"built_in"},
contains:[].concat(f,g,_,m,[l,{
begin:"\\b(deque|list|queue|priority_queue|pair|stack|vector|map|set|bitset|multiset|multimap|unordered_map|unordered_set|unordered_multiset|unordered_multimap|array|tuple|optional|variant|function)\\s*<(?!<)",
end:">",keywords:p,contains:["self",s]},{begin:e.IDENT_RE+"::",keywords:p},{
match:[/\b(?:enum(?:\s+(?:class|struct))?|class|struct|union)/,/\s+/,/\w+/],
className:{1:"keyword",3:"title.class"}}])}}})();hljs.registerLanguage("cpp",e)
})();/*! `ini` grammar compiled for Highlight.js 11.9.0 */
(()=>{var e=(()=>{"use strict";return e=>{const n=e.regex,a={className:"number",
relevance:0,variants:[{begin:/([+-]+)?[\d]+_[\d_]+/},{begin:e.NUMBER_RE}]
},s=e.COMMENT();s.variants=[{begin:/;/,end:/$/},{begin:/#/,end:/$/}];const i={
className:"variable",variants:[{begin:/\$[\w\d"][\w\d_]*/},{begin:/\$\{(.*?)\}/
}]},t={className:"literal",begin:/\bon|off|true|false|yes|no\b/},r={
className:"string",contains:[e.BACKSLASH_ESCAPE],variants:[{begin:"'''",
end:"'''",relevance:10},{begin:'"""',end:'"""',relevance:10},{begin:'"',end:'"'
},{begin:"'",end:"'"}]},l={begin:/\[/,end:/\]/,contains:[s,t,i,r,a,"self"],
relevance:0},c=n.either(/[A-Za-z0-9_-]+/,/"(\\"|[^"])*"/,/'[^']*'/);return{
name:"TOML, also INI",aliases:["toml"],case_insensitive:!0,illegal:/\S/,
contains:[s,{className:"section",begin:/\[+/,end:/\]+/},{
begin:n.concat(c,"(\\s*\\.\\s*",c,")*",n.lookahead(/\s*=\s*[^#\s]/)),
className:"attr",starts:{end:/$/,contains:[s,l,t,i,r,a]}}]}}})()
;hljs.registerLanguage("ini",e)})();/*! `java` grammar compiled for Highlight.js 11.9.0 */
(()=>{var e=(()=>{"use strict"
;var e="[0-9](_*[0-9])*",a=`\\.(${e})`,n="[0-9a-fA-F](_*[0-9a-fA-F])*",s={
className:"number",variants:[{
begin:`(\\b(${e})((${a})|\\.)?|(${a}))[eE][+-]?(${e})[fFdD]?\\b`},{
begin:`\\b(${e})((${a})[fFdD]?\\b|\\.([fFdD]\\b)?)`},{begin:`(${a})[fFdD]?\\b`
},{begin:`\\b(${e})[fFdD]\\b`},{
begin:`\\b0[xX]((${n})\\.?|(${n})?\\.(${n}))[pP][+-]?(${e})[fFdD]?\\b`},{
begin:"\\b(0|[1-9](_*[0-9])*)[lL]?\\b"},{begin:`\\b0[xX](${n})[lL]?\\b`},{
begin:"\\b0(_*[0-7])*[lL]?\\b"},{begin:"\\b0[bB][01](_*[01])*[lL]?\\b"}],
relevance:0};function t(e,a,n){return-1===n?"":e.replace(a,(s=>t(e,a,n-1)))}
return e=>{
const a=e.regex,n="[\xc0-\u02b8a-zA-Z_$][\xc0-\u02b8a-zA-Z_$0-9]*",i=n+t("(?:<"+n+"~~~(?:\\s*,\\s*"+n+"~~~)*>)?",/~~~/g,2),r={
keyword:["synchronized","abstract","private","var","static","if","const ","for","while","strictfp","finally","protected","import","native","final","void","enum","else","break","transient","catch","instanceof","volatile","case","assert","package","default","public","try","switch","continue","throws","protected","public","private","module","requires","exports","do","sealed","yield","permits"],
literal:["false","true","null"],
type:["char","boolean","long","float","int","byte","short","double"],
built_in:["super","this"]},l={className:"meta",begin:"@"+n,contains:[{
begin:/\(/,end:/\)/,contains:["self"]}]},c={className:"params",begin:/\(/,
end:/\)/,keywords:r,relevance:0,contains:[e.C_BLOCK_COMMENT_MODE],endsParent:!0}
;return{name:"Java",aliases:["jsp"],keywords:r,illegal:/<\/|#/,
contains:[e.COMMENT("/\\*\\*","\\*/",{relevance:0,contains:[{begin:/\w+@/,
relevance:0},{className:"doctag",begin:"@[A-Za-z]+"}]}),{
begin:/import java\.[a-z]+\./,keywords:"import",relevance:2
},e.C_LINE_COMMENT_MODE,e.C_BLOCK_COMMENT_MODE,{begin:/"""/,end:/"""/,
className:"string",contains:[e.BACKSLASH_ESCAPE]
},e.APOS_STRING_MODE,e.QUOTE_STRING_MODE,{
match:[/\b(?:class|interface|enum|extends|implements|new)/,/\s+/,n],className:{
1:"keyword",3:"title.class"}},{match:/non-sealed/,scope:"keyword"},{
begin:[a.concat(/(?!else)/,n),/\s+/,n,/\s+/,/=(?!=)/],className:{1:"type",
3:"variable",5:"operator"}},{begin:[/record/,/\s+/,n],className:{1:"keyword",
3:"title.class"},contains:[c,e.C_LINE_COMMENT_MODE,e.C_BLOCK_COMMENT_MODE]},{
beginKeywords:"new throw return else",relevance:0},{
begin:["(?:"+i+"\\s+)",e.UNDERSCORE_IDENT_RE,/\s*(?=\()/],className:{
2:"title.function"},keywords:r,contains:[{className:"params",begin:/\(/,
end:/\)/,keywords:r,relevance:0,
contains:[l,e.APOS_STRING_MODE,e.QUOTE_STRING_MODE,s,e.C_BLOCK_COMMENT_MODE]
},e.C_LINE_COMMENT_MODE,e.C_BLOCK_COMMENT_MODE]},s,l]}}})()
;hljs.registerLanguage("java",e)})();/*! `json` grammar compiled for Highlight.js 11.9.0 */
(()=>{var e=(()=>{"use strict";return e=>{const a=["true","false","null"],n={
scope:"literal",beginKeywords:a.join(" ")};return{name:"JSON",keywords:{
literal:a},contains:[{className:"attr",begin:/"(\\.|[^\\"\r\n])*"(?=\s*:)/,
relevance:1.01},{match:/[{}[\],:]/,className:"punctuation",relevance:0
},e.QUOTE_STRING_MODE,n,e.C_NUMBER_MODE,e.C_LINE_COMMENT_MODE,e.C_BLOCK_COMMENT_MODE],
illegal:"\\S"}}})();hljs.registerLanguage("json",e)})();/*! `plaintext` grammar compiled for Highlight.js 11.9.0 */
(()=>{var t=(()=>{"use strict";return t=>({name:"Plain text",
aliases:["text","txt"],disableAutodetect:!0})})()
;hljs.registerLanguage("plaintext",t)})();/*! `python` grammar compiled for Highlight.js 11.9.0 */
(()=>{var e=(()=>{"use strict";return e=>{
const n=e.regex,a=/[\p{XID_Start}_]\p{XID_Continue}*/u,s=["and","as","assert","async","await","break","case","class","continue","def","del","elif","else","except","finally","for","from","global","if","import","in","is","lambda","match","nonlocal|10","not","or","pass","raise","return","try","while","with","yield"],i={
$pattern:/[A-Za-z]\w+|__\w+__/,keyword:s,
built_in:["__import__","abs","all","any","ascii","bin","bool","breakpoint","bytearray","bytes","callable","chr","classmethod","compile","complex","delattr","dict","dir","divmod","enumerate","eval","exec","filter","float","format","frozenset","getattr","globals","hasattr","hash","help","hex","id","input","int","isinstance","issubclass","iter","len","list","locals","map","max","memoryview","min","next","object","oct","open","ord","pow","print","property","range","repr","reversed","round","set","setattr","slice","sorted","staticmethod","str","sum","super","tuple","type","vars","zip"],
literal:["__debug__","Ellipsis","False","None","NotImplemented","True"],
type:["Any","Callable","Coroutine","Dict","List","Literal","Generic","Optional","Sequence","Set","Tuple","Type","Union"]
},t={className:"meta",begin:/^(>>>|\.\.\.) /},r={className:"subst",begin:/\{/,
end:/\}/,keywords:i,illegal:/#/},l={begin:/\{\{/,relevance:0},b={
className:"string",contains:[e.BACKSLASH_ESCAPE],variants:[{
begin:/([uU]|[bB]|[rR]|[bB][rR]|[rR][bB])?'''/,end:/'''/,
contains:[e.BACKSLASH_ESCAPE,t],relevance:10},{
begin:/([uU]|[bB]|[rR]|[bB][rR]|[rR][bB])?"""/,end:/"""/,
contains:[e.BACKSLASH_ESCAPE,t],relevance:10},{
begin:/([fF][rR]|[rR][fF]|[fF])'''/,end:/'''/,
contains:[e.BACKSLASH_ESCAPE,t,l,r]},{begin:/([fF][rR]|[rR][fF]|[fF])"""/,
end:/"""/,contains:[e.BACKSLASH_ESCAPE,t,l,r]},{begin:/([uU]|[rR])'/,end:/'/,
relevance:10},{begin:/([uU]|[rR])"/,end:/"/,relevance:10},{
begin:/([bB]|[bB][rR]|[rR][bB])'/,end:/'/},{begin:/([bB]|[bB][rR]|[rR][bB])"/,
end:/"/},{begin:/([fF][rR]|[rR][fF]|[fF])'/,end:/'/,
contains:[e.BACKSLASH_ESCAPE,l,r]},{begin:/([fF][rR]|[rR][fF]|[fF])"/,end:/"/,
contains:[e.BACKSLASH_ESCAPE,l,r]},e.APOS_STRING_MODE,e.QUOTE_STRING_MODE]
},o="[0-9](_?[0-9])*",c=`(\\b(${o}))?\\.(${o})|\\b(${o})\\.`,d="\\b|"+s.join("|"),g={
className:"number",relevance:0,variants:[{
begin:`(\\b(${o})|(${c}))[eE][+-]?(${o})[jJ]?(?=${d})`},{begin:`(${c})[jJ]?`},{
begin:`\\b([1-9](_?[0-9])*|0+(_?0)*)[lLjJ]?(?=${d})`},{
begin:`\\b0[bB](_?[01])+[lL]?(?=${d})`},{begin:`\\b0[oO](_?[0-7])+[lL]?(?=${d})`
},{begin:`\\b0[xX](_?[0-9a-fA-F])+[lL]?(?=${d})`},{begin:`\\b(${o})[jJ](?=${d})`
}]},p={className:"comment",begin:n.lookahead(/# type:/),end:/$/,keywords:i,
contains:[{begin:/# type:/},{begin:/#/,end:/\b\B/,endsWithParent:!0}]},m={
className:"params",variants:[{className:"",begin:/\(\s*\)/,skip:!0},{begin:/\(/,
end:/\)/,excludeBegin:!0,excludeEnd:!0,keywords:i,
contains:["self",t,g,b,e.HASH_COMMENT_MODE]}]};return r.contains=[b,g,t],{
name:"Python",aliases:["py","gyp","ipython"],unicodeRegex:!0,keywords:i,
illegal:/(<\/|\?)|=>/,contains:[t,g,{begin:/\bself\b/},{beginKeywords:"if",
relevance:0},{match:/\bor\b/,scope:"keyword"},b,p,e.HASH_COMMENT_MODE,{
match:[/\bdef/,/\s+/,a],scope:{1:"keyword",3:"title.function"},contains:[m]},{
variants:[{match:[/\bclass/,/\s+/,a,/\s*/,/\(\s*/,a,/\s*\)/]},{
match:[/\bclass/,/\s+/,a]}],scope:{1:"keyword",3:"title.class",
6:"title.class.inherited"}},{className:"meta",begin:/^[\t ]*@/,end:/(?=#)|$/,
contains:[g,m,b]}]}}})();hljs.registerLanguage("python",e)})();/*! `sql` grammar compiled for Highlight.js 11.9.0 */
(()=>{var e=(()=>{"use strict";return e=>{
const r=e.regex,t=e.COMMENT("--","$"),n=["true","false","unknown"],a=["bigint","binary","blob","boolean","char","character","clob","date","dec","decfloat","decimal","float","int","integer","interval","nchar","nclob","national","numeric","real","row","smallint","time","timestamp","varchar","varying","varbinary"],i=["abs","acos","array_agg","asin","atan","avg","cast","ceil","ceiling","coalesce","corr","cos","cosh","count","covar_pop","covar_samp","cume_dist","dense_rank","deref","element","exp","extract","first_value","floor","json_array","json_arrayagg","json_exists","json_object","json_objectagg","json_query","json_table","json_table_primitive","json_value","lag","last_value","lead","listagg","ln","log","log10","lower","max","min","mod","nth_value","ntile","nullif","percent_rank","percentile_cont","percentile_disc","position","position_regex","power","rank","regr_avgx","regr_avgy","regr_count","regr_intercept","regr_r2","regr_slope","regr_sxx","regr_sxy","regr_syy","row_number","sin","sinh","sqrt","stddev_pop","stddev_samp","substring","substring_regex","sum","tan","tanh","translate","translate_regex","treat","trim","trim_array","unnest","upper","value_of","var_pop","var_samp","width_bucket"],s=["create table","insert into","primary key","foreign key","not null","alter table","add constraint","grouping sets","on overflow","character set","respect nulls","ignore nulls","nulls first","nulls last","depth first","breadth first"],o=i,c=["abs","acos","all","allocate","alter","and","any","are","array","array_agg","array_max_cardinality","as","asensitive","asin","asymmetric","at","atan","atomic","authorization","avg","begin","begin_frame","begin_partition","between","bigint","binary","blob","boolean","both","by","call","called","cardinality","cascaded","case","cast","ceil","ceiling","char","char_length","character","character_length","check","classifier","clob","close","coalesce","collate","collect","column","commit","condition","connect","constraint","contains","convert","copy","corr","corresponding","cos","cosh","count","covar_pop","covar_samp","create","cross","cube","cume_dist","current","current_catalog","current_date","current_default_transform_group","current_path","current_role","current_row","current_schema","current_time","current_timestamp","current_path","current_role","current_transform_group_for_type","current_user","cursor","cycle","date","day","deallocate","dec","decimal","decfloat","declare","default","define","delete","dense_rank","deref","describe","deterministic","disconnect","distinct","double","drop","dynamic","each","element","else","empty","end","end_frame","end_partition","end-exec","equals","escape","every","except","exec","execute","exists","exp","external","extract","false","fetch","filter","first_value","float","floor","for","foreign","frame_row","free","from","full","function","fusion","get","global","grant","group","grouping","groups","having","hold","hour","identity","in","indicator","initial","inner","inout","insensitive","insert","int","integer","intersect","intersection","interval","into","is","join","json_array","json_arrayagg","json_exists","json_object","json_objectagg","json_query","json_table","json_table_primitive","json_value","lag","language","large","last_value","lateral","lead","leading","left","like","like_regex","listagg","ln","local","localtime","localtimestamp","log","log10","lower","match","match_number","match_recognize","matches","max","member","merge","method","min","minute","mod","modifies","module","month","multiset","national","natural","nchar","nclob","new","no","none","normalize","not","nth_value","ntile","null","nullif","numeric","octet_length","occurrences_regex","of","offset","old","omit","on","one","only","open","or","order","out","outer","over","overlaps","overlay","parameter","partition","pattern","per","percent","percent_rank","percentile_cont","percentile_disc","period","portion","position","position_regex","power","precedes","precision","prepare","primary","procedure","ptf","range","rank","reads","real","recursive","ref","references","referencing","regr_avgx","regr_avgy","regr_count","regr_intercept","regr_r2","regr_slope","regr_sxx","regr_sxy","regr_syy","release","result","return","returns","revoke","right","rollback","rollup","row","row_number","rows","running","savepoint","scope","scroll","search","second","seek","select","sensitive","session_user","set","show","similar","sin","sinh","skip","smallint","some","specific","specifictype","sql","sqlexception","sqlstate","sqlwarning","sqrt","start","static","stddev_pop","stddev_samp","submultiset","subset","substring","substring_regex","succeeds","sum","symmetric","system","system_time","system_user","table","tablesample","tan","tanh","then","time","timestamp","timezone_hour","timezone_minute","to","trailing","translate","translate_regex","translation","treat","trigger","trim","trim_array","true","truncate","uescape","union","unique","unknown","unnest","update","upper","user","using","value","values","value_of","var_pop","var_samp","varbinary","varchar","varying","versioning","when","whenever","where","width_bucket","window","with","within","without","year","add","asc","collation","desc","final","first","last","view"].filter((e=>!i.includes(e))),l={
begin:r.concat(/\b/,r.either(...o),/\s*\(/),relevance:0,keywords:{built_in:o}}
;return{name:"SQL",case_insensitive:!0,illegal:/[{}]|<\//,keywords:{
$pattern:/\b[\w\.]+/,keyword:((e,{exceptions:r,when:t}={})=>{const n=t
;return r=r||[],e.map((e=>e.match(/\|\d+$/)||r.includes(e)?e:n(e)?e+"|0":e))
})(c,{when:e=>e.length<3}),literal:n,type:a,
built_in:["current_catalog","current_date","current_default_transform_group","current_path","current_role","current_schema","current_transform_group_for_type","current_user","session_user","system_time","system_user","current_time","localtime","current_timestamp","localtimestamp"]
},contains:[{begin:r.either(...s),relevance:0,keywords:{$pattern:/[\w\.]+/,
keyword:c.concat(s),literal:n,type:a}},{className:"type",
begin:r.either("double precision","large object","with timezone","without timezone")
},l,{className:"variable",begin:/@[a-z0-9][a-z0-9_]*/},{className:"string",
variants:[{begin:/'/,end:/'/,contains:[{begin:/''/}]}]},{begin:/"/,end:/"/,
contains:[{begin:/""/}]},e.C_NUMBER_MODE,e.C_BLOCK_COMMENT_MODE,t,{
className:"operator",begin:/[-+*/=%^~]|&&?|\|\|?|!=?|<(?:=>?|<|>)?|>[>=]?/,
relevance:0}]}}})();hljs.registerLanguage("sql",e)})();/*! `xml` grammar compiled for Highlight.js 11.9.0 */
(()=>{var e=(()=>{"use strict";return e=>{
const a=e.regex,n=a.concat(/[\p{L}_]/u,a.optional(/[\p{L}0-9_.-]*:/u),/[\p{L}0-9_.-]*/u),s={
className:"symbol",begin:/&[a-z]+;|&#[0-9]+;|&#x[a-f0-9]+;/},t={begin:/\s/,
contains:[{className:"keyword",begin:/#?[a-z_][a-z1-9_-]+/,illegal:/\n/}]
},i=e.inherit(t,{begin:/\(/,end:/\)/}),c=e.inherit(e.APOS_STRING_MODE,{
className:"string"}),l=e.inherit(e.QUOTE_STRING_MODE,{className:"string"}),r={
endsWithParent:!0,illegal:/</,relevance:0,contains:[{className:"attr",
begin:/[\p{L}0-9._:-]+/u,relevance:0},{begin:/=\s*/,relevance:0,contains:[{
className:"string",endsParent:!0,variants:[{begin:/"/,end:/"/,contains:[s]},{
begin:/'/,end:/'/,contains:[s]},{begin:/[^\s"'=<>`]+/}]}]}]};return{
name:"HTML, XML",
aliases:["html","xhtml","rss","atom","xjb","xsd","xsl","plist","wsf","svg"],
case_insensitive:!0,unicodeRegex:!0,contains:[{className:"meta",begin:/<![a-z]/,
end:/>/,relevance:10,contains:[t,l,c,i,{begin:/\[/,end:/\]/,contains:[{
className:"meta",begin:/<![a-z]/,end:/>/,contains:[t,i,l,c]}]}]
},e.COMMENT(/<!--/,/-->/,{relevance:10}),{begin:/<!\[CDATA\[/,end:/\]\]>/,
relevance:10},s,{className:"meta",end:/\?>/,variants:[{begin:/<\?xml/,
relevance:10,contains:[l]},{begin:/<\?[a-z][a-z0-9]+/}]},{className:"tag",
begin:/<style(?=\s|>)/,end:/>/,keywords:{name:"style"},contains:[r],starts:{
end:/<\/style>/,returnEnd:!0,subLanguage:["css","xml"]}},{className:"tag",
begin:/<script(?=\s|>)/,end:/>/,keywords:{name:"script"},contains:[r],starts:{
end:/<\/script>/,returnEnd:!0,subLanguage:["javascript","handlebars","xml"]}},{
className:"tag",begin:/<>|<\/>/},{className:"tag",
begin:a.concat(/</,a.lookahead(a.concat(n,a.either(/\/>/,/>/,/\s/)))),
end:/\/?>/,contains:[{className:"name",begin:n,relevance:0,starts:r}]},{
className:"tag",begin:a.concat(/<\//,a.lookahead(a.concat(n,/>/))),contains:[{
className:"name",begin:n,relevance:0},{begin:/>/,relevance:0,endsParent:!0}]}]}}
})();hljs.registerLanguage("xml",e)})();/*! `yaml` grammar compiled for Highlight.js 11.9.0 */
(()=>{var e=(()=>{"use strict";return e=>{
const n="true false yes no null",a="[\\w#;/?:@&=+$,.~*'()[\\]]+",s={
className:"string",relevance:0,variants:[{begin:/'/,end:/'/},{begin:/"/,end:/"/
},{begin:/\S+/}],contains:[e.BACKSLASH_ESCAPE,{className:"template-variable",
variants:[{begin:/\{\{/,end:/\}\}/},{begin:/%\{/,end:/\}/}]}]},i=e.inherit(s,{
variants:[{begin:/'/,end:/'/},{begin:/"/,end:/"/},{begin:/[^\s,{}[\]]+/}]}),l={
end:",",endsWithParent:!0,excludeEnd:!0,keywords:n,relevance:0},t={begin:/\{/,
end:/\}/,contains:[l],illegal:"\\n",relevance:0},g={begin:"\\[",end:"\\]",
contains:[l],illegal:"\\n",relevance:0},b=[{className:"attr",variants:[{
begin:/\w[\w :()\./-]*:(?=[ \t]|$)/},{begin:/"\w[\w :()\./-]*":(?=[ \t]|$)/},{
begin:/'\w[\w :()\./-]*':(?=[ \t]|$)/}]},{className:"meta",begin:"^---\\s*$",
relevance:10},{className:"string",
begin:"[\\|>]([1-9]?[+-])?[ ]*\\n( +)[^ ][^\\n]*\\n(\\2[^\\n]+\\n?)*"},{
begin:"<%[%=-]?",end:"[%-]?%>",subLanguage:"ruby",excludeBegin:!0,excludeEnd:!0,
relevance:0},{className:"type",begin:"!\\w+!"+a},{className:"type",
begin:"!<"+a+">"},{className:"type",begin:"!"+a},{className:"type",begin:"!!"+a
},{className:"meta",begin:"&"+e.UNDERSCORE_IDENT_RE+"$"},{className:"meta",
begin:"\\*"+e.UNDERSCORE_IDENT_RE+"$"},{className:"bullet",begin:"-(?=[ ]|$)",
relevance:0},e.HASH_COMMENT_MODE,{beginKeywords:n,keywords:{literal:n}},{
className:"number",
begin:"\\b[0-9]{4}(-[0-9][0-9]){0,2}([Tt \\t][0-9][0-9]?(:[0-9][0-9]){2})?(\\.[0-9]*)?([ \\t])*(Z|[-+][0-9][0-9]?(:[0-9][0-9])?)?\\b"
},{className:"number",begin:e.C_NUMBER_RE+"\\b",relevance:0},t,g,s],r=[...b]
;return r.pop(),r.push(i),l.contains=r,{name:"YAML",case_insensitive:!0,
aliases:["yml"],contains:b}}})();hljs.registerLanguage("yaml",e)})();
(function(){'use strict';var f,g=[];function l(a){g.push(a);1==g.length&&f()}function m(){for(;g.length;)g[0](),g.shift()}f=function(){setTimeout(m)};function n(a){this.a=p;this.b=void 0;this.f=[];var b=this;try{a(function(a){q(b,a)},function(a){r(b,a)})}catch(c){r(b,c)}}var p=2;function t(a){return new n(function(b,c){c(a)})}function u(a){return new n(function(b){b(a)})}function q(a,b){if(a.a==p){if(b==a)throw new TypeError;var c=!1;try{var d=b&&b.then;if(null!=b&&"object"==typeof b&&"function"==typeof d){d.call(b,function(b){c||q(a,b);c=!0},function(b){c||r(a,b);c=!0});return}}catch(e){c||r(a,e);return}a.a=0;a.b=b;v(a)}}
    function r(a,b){if(a.a==p){if(b==a)throw new TypeError;a.a=1;a.b=b;v(a)}}function v(a){l(function(){if(a.a!=p)for(;a.f.length;){var b=a.f.shift(),c=b[0],d=b[1],e=b[2],b=b[3];try{0==a.a?"function"==typeof c?e(c.call(void 0,a.b)):e(a.b):1==a.a&&("function"==typeof d?e(d.call(void 0,a.b)):b(a.b))}catch(h){b(h)}}})}n.prototype.g=function(a){return this.c(void 0,a)};n.prototype.c=function(a,b){var c=this;return new n(function(d,e){c.f.push([a,b,d,e]);v(c)})};
    function w(a){return new n(function(b,c){function d(c){return function(d){h[c]=d;e+=1;e==a.length&&b(h)}}var e=0,h=[];0==a.length&&b(h);for(var k=0;k<a.length;k+=1)u(a[k]).c(d(k),c)})}function x(a){return new n(function(b,c){for(var d=0;d<a.length;d+=1)u(a[d]).c(b,c)})};window.Promise||(window.Promise=n,window.Promise.resolve=u,window.Promise.reject=t,window.Promise.race=x,window.Promise.all=w,window.Promise.prototype.then=n.prototype.c,window.Promise.prototype["catch"]=n.prototype.g);}());

(function(){function l(a,b){document.addEventListener?a.addEventListener("scroll",b,!1):a.attachEvent("scroll",b)}function m(a){document.body?a():document.addEventListener?document.addEventListener("DOMContentLoaded",function c(){document.removeEventListener("DOMContentLoaded",c);a()}):document.attachEvent("onreadystatechange",function k(){if("interactive"==document.readyState||"complete"==document.readyState)document.detachEvent("onreadystatechange",k),a()})};function v(a){this.a=document.createElement("div");this.a.setAttribute("aria-hidden","true");this.a.appendChild(document.createTextNode(a));this.b=document.createElement("span");this.c=document.createElement("span");this.h=document.createElement("span");this.f=document.createElement("span");this.g=-1;this.b.style.cssText="max-width:none;display:inline-block;position:absolute;height:100%;width:100%;overflow:scroll;font-size:16px;";this.c.style.cssText="max-width:none;display:inline-block;position:absolute;height:100%;width:100%;overflow:scroll;font-size:16px;";
    this.f.style.cssText="max-width:none;display:inline-block;position:absolute;height:100%;width:100%;overflow:scroll;font-size:16px;";this.h.style.cssText="display:inline-block;width:200%;height:200%;font-size:16px;max-width:none;";this.b.appendChild(this.h);this.c.appendChild(this.f);this.a.appendChild(this.b);this.a.appendChild(this.c)}
    function w(a,b){a.a.style.cssText="max-width:none;min-width:20px;min-height:20px;display:inline-block;overflow:hidden;position:absolute;width:auto;margin:0;padding:0;top:-999px;left:-999px;white-space:nowrap;font:"+b+";"}function y(a){var b=a.a.offsetWidth,c=b+100;a.f.style.width=c+"px";a.c.scrollLeft=c;a.b.scrollLeft=a.b.scrollWidth+100;return a.g!==b?(a.g=b,!0):!1}function z(a,b){function c(){var a=k;y(a)&&null!==a.a.parentNode&&b(a.g)}var k=a;l(a.b,c);l(a.c,c);y(a)};function A(a,b){var c=b||{};this.family=a;this.style=c.style||"normal";this.weight=c.weight||"normal";this.stretch=c.stretch||"normal"}var B=null,C=null,G=!!window.FontFace;function H(){if(null===C){var a=document.createElement("div");try{a.style.font="condensed 100px sans-serif"}catch(b){}C=""!==a.style.font}return C}function I(a,b){return[a.style,a.weight,H()?a.stretch:"","100px",b].join(" ")}
    A.prototype.load=function(a,b){var c=this,k=a||"BESbswy",x=b||3E3,D=(new Date).getTime();return new Promise(function(a,b){if(G){var J=new Promise(function(a,b){function e(){(new Date).getTime()-D>=x?b():document.fonts.load(I(c,c.family),k).then(function(c){1<=c.length?a():setTimeout(e,25)},function(){b()})}e()}),K=new Promise(function(a,c){setTimeout(c,x)});Promise.race([K,J]).then(function(){a(c)},function(){b(c)})}else m(function(){function q(){var b;if(b=-1!=f&&-1!=g||-1!=f&&-1!=h||-1!=g&&-1!=
            h)(b=f!=g&&f!=h&&g!=h)||(null===B&&(b=/AppleWebKit\/([0-9]+)(?:\.([0-9]+))/.exec(window.navigator.userAgent),B=!!b&&(536>parseInt(b[1],10)||536===parseInt(b[1],10)&&11>=parseInt(b[2],10))),b=B&&(f==r&&g==r&&h==r||f==t&&g==t&&h==t||f==u&&g==u&&h==u)),b=!b;b&&(null!==d.parentNode&&d.parentNode.removeChild(d),clearTimeout(F),a(c))}function E(){if((new Date).getTime()-D>=x)null!==d.parentNode&&d.parentNode.removeChild(d),b(c);else{var a=document.hidden;if(!0===a||void 0===a)f=e.a.offsetWidth,g=n.a.offsetWidth,
        h=p.a.offsetWidth,q();F=setTimeout(E,50)}}var e=new v(k),n=new v(k),p=new v(k),f=-1,g=-1,h=-1,r=-1,t=-1,u=-1,d=document.createElement("div"),F=0;d.dir="ltr";w(e,I(c,"sans-serif"));w(n,I(c,"serif"));w(p,I(c,"monospace"));d.appendChild(e.a);d.appendChild(n.a);d.appendChild(p.a);document.body.appendChild(d);r=e.a.offsetWidth;t=n.a.offsetWidth;u=p.a.offsetWidth;E();z(e,function(a){f=a;q()});w(e,I(c,'"'+c.family+'",sans-serif'));z(n,function(a){g=a;q()});w(n,I(c,'"'+c.family+'",serif'));z(p,function(a){h=
        a;q()});w(p,I(c,'"'+c.family+'",monospace'))})})};window.FontFaceObserver=A;window.FontFaceObserver.prototype.check=window.FontFaceObserver.prototype.load=A.prototype.load;"undefined"!==typeof module&&(module.exports=window.FontFaceObserver);}());
/*!
 * Fuse.js v3.1.0 - Lightweight fuzzy-search (http://fusejs.io)
 *
 * Copyright (c) 2012-2017 Kirollos Risk (http://kiro.me)
 * All Rights Reserved. Apache Software License 2.0
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 */
(function webpackUniversalModuleDefinition(root, factory) {
    if(typeof exports === 'object' && typeof module === 'object')
        module.exports = factory();
    else if(typeof define === 'function' && define.amd)
        define("Fuse", [], factory);
    else if(typeof exports === 'object')
        exports["Fuse"] = factory();
    else
        root["Fuse"] = factory();
})(this, function() {
    return /******/ (function(modules) { // webpackBootstrap
        /******/ 	// The module cache
        /******/ 	var installedModules = {};
        /******/
        /******/ 	// The require function
        /******/ 	function __webpack_require__(moduleId) {
            /******/
            /******/ 		// Check if module is in cache
            /******/ 		if(installedModules[moduleId]) {
                /******/ 			return installedModules[moduleId].exports;
                /******/ 		}
            /******/ 		// Create a new module (and put it into the cache)
            /******/ 		var module = installedModules[moduleId] = {
                /******/ 			i: moduleId,
                /******/ 			l: false,
                /******/ 			exports: {}
                /******/ 		};
            /******/
            /******/ 		// Execute the module function
            /******/ 		modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);
            /******/
            /******/ 		// Flag the module as loaded
            /******/ 		module.l = true;
            /******/
            /******/ 		// Return the exports of the module
            /******/ 		return module.exports;
            /******/ 	}
        /******/
        /******/
        /******/ 	// expose the modules object (__webpack_modules__)
        /******/ 	__webpack_require__.m = modules;
        /******/
        /******/ 	// expose the module cache
        /******/ 	__webpack_require__.c = installedModules;
        /******/
        /******/ 	// identity function for calling harmony imports with the correct context
        /******/ 	__webpack_require__.i = function(value) { return value; };
        /******/
        /******/ 	// define getter function for harmony exports
        /******/ 	__webpack_require__.d = function(exports, name, getter) {
            /******/ 		if(!__webpack_require__.o(exports, name)) {
                /******/ 			Object.defineProperty(exports, name, {
                    /******/ 				configurable: false,
                    /******/ 				enumerable: true,
                    /******/ 				get: getter
                    /******/ 			});
                /******/ 		}
            /******/ 	};
        /******/
        /******/ 	// getDefaultExport function for compatibility with non-harmony modules
        /******/ 	__webpack_require__.n = function(module) {
            /******/ 		var getter = module && module.__esModule ?
                /******/ 			function getDefault() { return module['default']; } :
                /******/ 			function getModuleExports() { return module; };
            /******/ 		__webpack_require__.d(getter, 'a', getter);
            /******/ 		return getter;
            /******/ 	};
        /******/
        /******/ 	// Object.prototype.hasOwnProperty.call
        /******/ 	__webpack_require__.o = function(object, property) { return Object.prototype.hasOwnProperty.call(object, property); };
        /******/
        /******/ 	// __webpack_public_path__
        /******/ 	__webpack_require__.p = "";
        /******/
        /******/ 	// Load entry module and return exports
        /******/ 	return __webpack_require__(__webpack_require__.s = 8);
        /******/ })
    /************************************************************************/
    /******/ ([
        /* 0 */
        /***/ (function(module, exports, __webpack_require__) {

            "use strict";


            module.exports = function (obj) {
                return Object.prototype.toString.call(obj) === '[object Array]';
            };

            /***/ }),
        /* 1 */
        /***/ (function(module, exports, __webpack_require__) {

            "use strict";


            var _createClass = function () { function defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } } return function (Constructor, protoProps, staticProps) { if (protoProps) defineProperties(Constructor.prototype, protoProps); if (staticProps) defineProperties(Constructor, staticProps); return Constructor; }; }();

            function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

            var bitapRegexSearch = __webpack_require__(5);
            var bitapSearch = __webpack_require__(7);
            var patternAlphabet = __webpack_require__(4);

            var Bitap = function () {
                function Bitap(pattern, _ref) {
                    var _ref$location = _ref.location,
                        location = _ref$location === undefined ? 0 : _ref$location,
                        _ref$distance = _ref.distance,
                        distance = _ref$distance === undefined ? 100 : _ref$distance,
                        _ref$threshold = _ref.threshold,
                        threshold = _ref$threshold === undefined ? 0.6 : _ref$threshold,
                        _ref$maxPatternLength = _ref.maxPatternLength,
                        maxPatternLength = _ref$maxPatternLength === undefined ? 32 : _ref$maxPatternLength,
                        _ref$isCaseSensitive = _ref.isCaseSensitive,
                        isCaseSensitive = _ref$isCaseSensitive === undefined ? false : _ref$isCaseSensitive,
                        _ref$tokenSeparator = _ref.tokenSeparator,
                        tokenSeparator = _ref$tokenSeparator === undefined ? / +/g : _ref$tokenSeparator,
                        _ref$findAllMatches = _ref.findAllMatches,
                        findAllMatches = _ref$findAllMatches === undefined ? false : _ref$findAllMatches,
                        _ref$minMatchCharLeng = _ref.minMatchCharLength,
                        minMatchCharLength = _ref$minMatchCharLeng === undefined ? 1 : _ref$minMatchCharLeng;

                    _classCallCheck(this, Bitap);

                    this.options = {
                        location: location,
                        distance: distance,
                        threshold: threshold,
                        maxPatternLength: maxPatternLength,
                        isCaseSensitive: isCaseSensitive,
                        tokenSeparator: tokenSeparator,
                        findAllMatches: findAllMatches,
                        minMatchCharLength: minMatchCharLength
                    };

                    this.pattern = this.options.isCaseSensitive ? pattern : pattern.toLowerCase();

                    if (this.pattern.length <= maxPatternLength) {
                        this.patternAlphabet = patternAlphabet(this.pattern);
                    }
                }

                _createClass(Bitap, [{
                    key: 'search',
                    value: function search(text) {
                        if (!this.options.isCaseSensitive) {
                            text = text.toLowerCase();
                        }

                        // Exact match
                        if (this.pattern === text) {
                            return {
                                isMatch: true,
                                score: 0,
                                matchedIndices: [[0, text.length - 1]]
                            };
                        }

                        // When pattern length is greater than the machine word length, just do a a regex comparison
                        var _options = this.options,
                            maxPatternLength = _options.maxPatternLength,
                            tokenSeparator = _options.tokenSeparator;

                        if (this.pattern.length > maxPatternLength) {
                            return bitapRegexSearch(text, this.pattern, tokenSeparator);
                        }

                        // Otherwise, use Bitap algorithm
                        var _options2 = this.options,
                            location = _options2.location,
                            distance = _options2.distance,
                            threshold = _options2.threshold,
                            findAllMatches = _options2.findAllMatches,
                            minMatchCharLength = _options2.minMatchCharLength;

                        return bitapSearch(text, this.pattern, this.patternAlphabet, {
                            location: location,
                            distance: distance,
                            threshold: threshold,
                            findAllMatches: findAllMatches,
                            minMatchCharLength: minMatchCharLength
                        });
                    }
                }]);

                return Bitap;
            }();

// let x = new Bitap("od mn war", {})
// let result = x.search("Old Man's War")
// console.log(result)

            module.exports = Bitap;

            /***/ }),
        /* 2 */
        /***/ (function(module, exports, __webpack_require__) {

            "use strict";


            var isArray = __webpack_require__(0);

            var deepValue = function deepValue(obj, path, list) {
                if (!path) {
                    // If there's no path left, we've gotten to the object we care about.
                    list.push(obj);
                } else {
                    var dotIndex = path.indexOf('.');
                    var firstSegment = path;
                    var remaining = null;

                    if (dotIndex !== -1) {
                        firstSegment = path.slice(0, dotIndex);
                        remaining = path.slice(dotIndex + 1);
                    }

                    var value = obj[firstSegment];

                    if (value !== null && value !== undefined) {
                        if (!remaining && (typeof value === 'string' || typeof value === 'number')) {
                            list.push(value.toString());
                        } else if (isArray(value)) {
                            // Search each item in the array.
                            for (var i = 0, len = value.length; i < len; i += 1) {
                                deepValue(value[i], remaining, list);
                            }
                        } else if (remaining) {
                            // An object. Recurse further.
                            deepValue(value, remaining, list);
                        }
                    }
                }

                return list;
            };

            module.exports = function (obj, path) {
                return deepValue(obj, path, []);
            };

            /***/ }),
        /* 3 */
        /***/ (function(module, exports, __webpack_require__) {

            "use strict";


            module.exports = function () {
                var matchmask = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : [];
                var minMatchCharLength = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 1;

                var matchedIndices = [];
                var start = -1;
                var end = -1;
                var i = 0;

                for (var len = matchmask.length; i < len; i += 1) {
                    var match = matchmask[i];
                    if (match && start === -1) {
                        start = i;
                    } else if (!match && start !== -1) {
                        end = i - 1;
                        if (end - start + 1 >= minMatchCharLength) {
                            matchedIndices.push([start, end]);
                        }
                        start = -1;
                    }
                }

                // (i-1 - start) + 1 => i - start
                if (matchmask[i - 1] && i - start >= minMatchCharLength) {
                    matchedIndices.push([start, i - 1]);
                }

                return matchedIndices;
            };

            /***/ }),
        /* 4 */
        /***/ (function(module, exports, __webpack_require__) {

            "use strict";


            module.exports = function (pattern) {
                var mask = {};
                var len = pattern.length;

                for (var i = 0; i < len; i += 1) {
                    mask[pattern.charAt(i)] = 0;
                }

                for (var _i = 0; _i < len; _i += 1) {
                    mask[pattern.charAt(_i)] |= 1 << len - _i - 1;
                }

                return mask;
            };

            /***/ }),
        /* 5 */
        /***/ (function(module, exports, __webpack_require__) {

            "use strict";


            var SPECIAL_CHARS_REGEX = /[\-\[\]\/\{\}\(\)\*\+\?\.\\\^\$\|]/g;

            module.exports = function (text, pattern) {
                var tokenSeparator = arguments.length > 2 && arguments[2] !== undefined ? arguments[2] : / +/g;

                var regex = new RegExp(pattern.replace(SPECIAL_CHARS_REGEX, '\\$&').replace(tokenSeparator, '|'));
                var matches = text.match(regex);
                var isMatch = !!matches;
                var matchedIndices = [];

                if (isMatch) {
                    for (var i = 0, matchesLen = matches.length; i < matchesLen; i += 1) {
                        var match = matches[i];
                        matchedIndices.push([text.indexOf(match), match.length - 1]);
                    }
                }

                return {
                    // TODO: revisit this score
                    score: isMatch ? 0.5 : 1,
                    isMatch: isMatch,
                    matchedIndices: matchedIndices
                };
            };

            /***/ }),
        /* 6 */
        /***/ (function(module, exports, __webpack_require__) {

            "use strict";


            module.exports = function (pattern, _ref) {
                var _ref$errors = _ref.errors,
                    errors = _ref$errors === undefined ? 0 : _ref$errors,
                    _ref$currentLocation = _ref.currentLocation,
                    currentLocation = _ref$currentLocation === undefined ? 0 : _ref$currentLocation,
                    _ref$expectedLocation = _ref.expectedLocation,
                    expectedLocation = _ref$expectedLocation === undefined ? 0 : _ref$expectedLocation,
                    _ref$distance = _ref.distance,
                    distance = _ref$distance === undefined ? 100 : _ref$distance;

                var accuracy = errors / pattern.length;
                var proximity = Math.abs(expectedLocation - currentLocation);

                if (!distance) {
                    // Dodge divide by zero error.
                    return proximity ? 1.0 : accuracy;
                }

                return accuracy + proximity / distance;
            };

            /***/ }),
        /* 7 */
        /***/ (function(module, exports, __webpack_require__) {

            "use strict";


            var bitapScore = __webpack_require__(6);
            var matchedIndices = __webpack_require__(3);

            module.exports = function (text, pattern, patternAlphabet, _ref) {
                var _ref$location = _ref.location,
                    location = _ref$location === undefined ? 0 : _ref$location,
                    _ref$distance = _ref.distance,
                    distance = _ref$distance === undefined ? 100 : _ref$distance,
                    _ref$threshold = _ref.threshold,
                    threshold = _ref$threshold === undefined ? 0.6 : _ref$threshold,
                    _ref$findAllMatches = _ref.findAllMatches,
                    findAllMatches = _ref$findAllMatches === undefined ? false : _ref$findAllMatches,
                    _ref$minMatchCharLeng = _ref.minMatchCharLength,
                    minMatchCharLength = _ref$minMatchCharLeng === undefined ? 1 : _ref$minMatchCharLeng;

                var expectedLocation = location;
                // Set starting location at beginning text and initialize the alphabet.
                var textLen = text.length;
                // Highest score beyond which we give up.
                var currentThreshold = threshold;
                // Is there a nearby exact match? (speedup)
                var bestLocation = text.indexOf(pattern, expectedLocation);

                var patternLen = pattern.length;

                // a mask of the matches
                var matchMask = [];
                for (var i = 0; i < textLen; i += 1) {
                    matchMask[i] = 0;
                }

                if (bestLocation !== -1) {
                    var score = bitapScore(pattern, {
                        errors: 0,
                        currentLocation: bestLocation,
                        expectedLocation: expectedLocation,
                        distance: distance
                    });
                    currentThreshold = Math.min(score, currentThreshold);

                    // What about in the other direction? (speed up)
                    bestLocation = text.lastIndexOf(pattern, expectedLocation + patternLen);

                    if (bestLocation !== -1) {
                        var _score = bitapScore(pattern, {
                            errors: 0,
                            currentLocation: bestLocation,
                            expectedLocation: expectedLocation,
                            distance: distance
                        });
                        currentThreshold = Math.min(_score, currentThreshold);
                    }
                }

                // Reset the best location
                bestLocation = -1;

                var lastBitArr = [];
                var finalScore = 1;
                var binMax = patternLen + textLen;

                var mask = 1 << patternLen - 1;

                for (var _i = 0; _i < patternLen; _i += 1) {
                    // Scan for the best match; each iteration allows for one more error.
                    // Run a binary search to determine how far from the match location we can stray
                    // at this error level.
                    var binMin = 0;
                    var binMid = binMax;

                    while (binMin < binMid) {
                        var _score3 = bitapScore(pattern, {
                            errors: _i,
                            currentLocation: expectedLocation + binMid,
                            expectedLocation: expectedLocation,
                            distance: distance
                        });

                        if (_score3 <= currentThreshold) {
                            binMin = binMid;
                        } else {
                            binMax = binMid;
                        }

                        binMid = Math.floor((binMax - binMin) / 2 + binMin);
                    }

                    // Use the result from this iteration as the maximum for the next.
                    binMax = binMid;

                    var start = Math.max(1, expectedLocation - binMid + 1);
                    var finish = findAllMatches ? textLen : Math.min(expectedLocation + binMid, textLen) + patternLen;

                    // Initialize the bit array
                    var bitArr = Array(finish + 2);

                    bitArr[finish + 1] = (1 << _i) - 1;

                    for (var j = finish; j >= start; j -= 1) {
                        var currentLocation = j - 1;
                        var charMatch = patternAlphabet[text.charAt(currentLocation)];

                        if (charMatch) {
                            matchMask[currentLocation] = 1;
                        }

                        // First pass: exact match
                        bitArr[j] = (bitArr[j + 1] << 1 | 1) & charMatch;

                        // Subsequent passes: fuzzy match
                        if (_i !== 0) {
                            bitArr[j] |= (lastBitArr[j + 1] | lastBitArr[j]) << 1 | 1 | lastBitArr[j + 1];
                        }

                        if (bitArr[j] & mask) {
                            finalScore = bitapScore(pattern, {
                                errors: _i,
                                currentLocation: currentLocation,
                                expectedLocation: expectedLocation,
                                distance: distance
                            });

                            // This match will almost certainly be better than any existing match.
                            // But check anyway.
                            if (finalScore <= currentThreshold) {
                                // Indeed it is
                                currentThreshold = finalScore;
                                bestLocation = currentLocation;

                                // Already passed `loc`, downhill from here on in.
                                if (bestLocation <= expectedLocation) {
                                    break;
                                }

                                // When passing `bestLocation`, don't exceed our current distance from `expectedLocation`.
                                start = Math.max(1, 2 * expectedLocation - bestLocation);
                            }
                        }
                    }

                    // No hope for a (better) match at greater error levels.
                    var _score2 = bitapScore(pattern, {
                        errors: _i + 1,
                        currentLocation: expectedLocation,
                        expectedLocation: expectedLocation,
                        distance: distance
                    });

                    if (_score2 > currentThreshold) {
                        break;
                    }

                    lastBitArr = bitArr;
                }

                // Count exact matches (those with a score of 0) to be "almost" exact
                return {
                    isMatch: bestLocation >= 0,
                    score: finalScore === 0 ? 0.001 : finalScore,
                    matchedIndices: matchedIndices(matchMask, minMatchCharLength)
                };
            };

            /***/ }),
        /* 8 */
        /***/ (function(module, exports, __webpack_require__) {

            "use strict";


            var _createClass = function () { function defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } } return function (Constructor, protoProps, staticProps) { if (protoProps) defineProperties(Constructor.prototype, protoProps); if (staticProps) defineProperties(Constructor, staticProps); return Constructor; }; }();

            function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

            var Bitap = __webpack_require__(1);
            var deepValue = __webpack_require__(2);
            var isArray = __webpack_require__(0);

            var Fuse = function () {
                function Fuse(list, _ref) {
                    var _ref$location = _ref.location,
                        location = _ref$location === undefined ? 0 : _ref$location,
                        _ref$distance = _ref.distance,
                        distance = _ref$distance === undefined ? 100 : _ref$distance,
                        _ref$threshold = _ref.threshold,
                        threshold = _ref$threshold === undefined ? 0.6 : _ref$threshold,
                        _ref$maxPatternLength = _ref.maxPatternLength,
                        maxPatternLength = _ref$maxPatternLength === undefined ? 32 : _ref$maxPatternLength,
                        _ref$caseSensitive = _ref.caseSensitive,
                        caseSensitive = _ref$caseSensitive === undefined ? false : _ref$caseSensitive,
                        _ref$tokenSeparator = _ref.tokenSeparator,
                        tokenSeparator = _ref$tokenSeparator === undefined ? / +/g : _ref$tokenSeparator,
                        _ref$findAllMatches = _ref.findAllMatches,
                        findAllMatches = _ref$findAllMatches === undefined ? false : _ref$findAllMatches,
                        _ref$minMatchCharLeng = _ref.minMatchCharLength,
                        minMatchCharLength = _ref$minMatchCharLeng === undefined ? 1 : _ref$minMatchCharLeng,
                        _ref$id = _ref.id,
                        id = _ref$id === undefined ? null : _ref$id,
                        _ref$keys = _ref.keys,
                        keys = _ref$keys === undefined ? [] : _ref$keys,
                        _ref$shouldSort = _ref.shouldSort,
                        shouldSort = _ref$shouldSort === undefined ? true : _ref$shouldSort,
                        _ref$getFn = _ref.getFn,
                        getFn = _ref$getFn === undefined ? deepValue : _ref$getFn,
                        _ref$sortFn = _ref.sortFn,
                        sortFn = _ref$sortFn === undefined ? function (a, b) {
                            return a.score - b.score;
                        } : _ref$sortFn,
                        _ref$tokenize = _ref.tokenize,
                        tokenize = _ref$tokenize === undefined ? false : _ref$tokenize,
                        _ref$matchAllTokens = _ref.matchAllTokens,
                        matchAllTokens = _ref$matchAllTokens === undefined ? false : _ref$matchAllTokens,
                        _ref$includeMatches = _ref.includeMatches,
                        includeMatches = _ref$includeMatches === undefined ? false : _ref$includeMatches,
                        _ref$includeScore = _ref.includeScore,
                        includeScore = _ref$includeScore === undefined ? false : _ref$includeScore,
                        _ref$verbose = _ref.verbose,
                        verbose = _ref$verbose === undefined ? false : _ref$verbose;

                    _classCallCheck(this, Fuse);

                    this.options = {
                        location: location,
                        distance: distance,
                        threshold: threshold,
                        maxPatternLength: maxPatternLength,
                        isCaseSensitive: caseSensitive,
                        tokenSeparator: tokenSeparator,
                        findAllMatches: findAllMatches,
                        minMatchCharLength: minMatchCharLength,
                        id: id,
                        keys: keys,
                        includeMatches: includeMatches,
                        includeScore: includeScore,
                        shouldSort: shouldSort,
                        getFn: getFn,
                        sortFn: sortFn,
                        verbose: verbose,
                        tokenize: tokenize,
                        matchAllTokens: matchAllTokens
                    };

                    this.setCollection(list);
                }

                _createClass(Fuse, [{
                    key: 'setCollection',
                    value: function setCollection(list) {
                        this.list = list;
                        return list;
                    }
                }, {
                    key: 'search',
                    value: function search(pattern) {
                        this._log('---------\nSearch pattern: "' + pattern + '"');

                        var _prepareSearchers2 = this._prepareSearchers(pattern),
                            tokenSearchers = _prepareSearchers2.tokenSearchers,
                            fullSearcher = _prepareSearchers2.fullSearcher;

                        var _search2 = this._search(tokenSearchers, fullSearcher),
                            weights = _search2.weights,
                            results = _search2.results;

                        this._computeScore(weights, results);

                        if (this.options.shouldSort) {
                            this._sort(results);
                        }

                        return this._format(results);
                    }
                }, {
                    key: '_prepareSearchers',
                    value: function _prepareSearchers() {
                        var pattern = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : '';

                        var tokenSearchers = [];

                        if (this.options.tokenize) {
                            // Tokenize on the separator
                            var tokens = pattern.split(this.options.tokenSeparator);
                            for (var i = 0, len = tokens.length; i < len; i += 1) {
                                tokenSearchers.push(new Bitap(tokens[i], this.options));
                            }
                        }

                        var fullSearcher = new Bitap(pattern, this.options);

                        return { tokenSearchers: tokenSearchers, fullSearcher: fullSearcher };
                    }
                }, {
                    key: '_search',
                    value: function _search() {
                        var tokenSearchers = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : [];
                        var fullSearcher = arguments[1];

                        var list = this.list;
                        var resultMap = {};
                        var results = [];

                        // Check the first item in the list, if it's a string, then we assume
                        // that every item in the list is also a string, and thus it's a flattened array.
                        if (typeof list[0] === 'string') {
                            // Iterate over every item
                            for (var i = 0, len = list.length; i < len; i += 1) {
                                this._analyze({
                                    key: '',
                                    value: list[i],
                                    record: i,
                                    index: i
                                }, {
                                    resultMap: resultMap,
                                    results: results,
                                    tokenSearchers: tokenSearchers,
                                    fullSearcher: fullSearcher
                                });
                            }

                            return { weights: null, results: results };
                        }

                        // Otherwise, the first item is an Object (hopefully), and thus the searching
                        // is done on the values of the keys of each item.
                        var weights = {};
                        for (var _i = 0, _len = list.length; _i < _len; _i += 1) {
                            var item = list[_i];
                            // Iterate over every key
                            for (var j = 0, keysLen = this.options.keys.length; j < keysLen; j += 1) {
                                var key = this.options.keys[j];
                                if (typeof key !== 'string') {
                                    weights[key.name] = {
                                        weight: 1 - key.weight || 1
                                    };
                                    if (key.weight <= 0 || key.weight > 1) {
                                        throw new Error('Key weight has to be > 0 and <= 1');
                                    }
                                    key = key.name;
                                } else {
                                    weights[key] = {
                                        weight: 1
                                    };
                                }

                                this._analyze({
                                    key: key,
                                    value: this.options.getFn(item, key),
                                    record: item,
                                    index: _i
                                }, {
                                    resultMap: resultMap,
                                    results: results,
                                    tokenSearchers: tokenSearchers,
                                    fullSearcher: fullSearcher
                                });
                            }
                        }

                        return { weights: weights, results: results };
                    }
                }, {
                    key: '_analyze',
                    value: function _analyze(_ref2, _ref3) {
                        var key = _ref2.key,
                            _ref2$arrayIndex = _ref2.arrayIndex,
                            arrayIndex = _ref2$arrayIndex === undefined ? -1 : _ref2$arrayIndex,
                            value = _ref2.value,
                            record = _ref2.record,
                            index = _ref2.index;
                        var _ref3$tokenSearchers = _ref3.tokenSearchers,
                            tokenSearchers = _ref3$tokenSearchers === undefined ? [] : _ref3$tokenSearchers,
                            _ref3$fullSearcher = _ref3.fullSearcher,
                            fullSearcher = _ref3$fullSearcher === undefined ? [] : _ref3$fullSearcher,
                            _ref3$resultMap = _ref3.resultMap,
                            resultMap = _ref3$resultMap === undefined ? {} : _ref3$resultMap,
                            _ref3$results = _ref3.results,
                            results = _ref3$results === undefined ? [] : _ref3$results;

                        // Check if the texvaluet can be searched
                        if (value === undefined || value === null) {
                            return;
                        }

                        var exists = false;
                        var averageScore = -1;
                        var numTextMatches = 0;

                        if (typeof value === 'string') {
                            this._log('\nKey: ' + (key === '' ? '-' : key));

                            var mainSearchResult = fullSearcher.search(value);
                            this._log('Full text: "' + value + '", score: ' + mainSearchResult.score);

                            if (this.options.tokenize) {
                                var words = value.split(this.options.tokenSeparator);
                                var scores = [];

                                for (var i = 0; i < tokenSearchers.length; i += 1) {
                                    var tokenSearcher = tokenSearchers[i];

                                    this._log('\nPattern: "' + tokenSearcher.pattern + '"');

                                    // let tokenScores = []
                                    var hasMatchInText = false;

                                    for (var j = 0; j < words.length; j += 1) {
                                        var word = words[j];
                                        var tokenSearchResult = tokenSearcher.search(word);
                                        var obj = {};
                                        if (tokenSearchResult.isMatch) {
                                            obj[word] = tokenSearchResult.score;
                                            exists = true;
                                            hasMatchInText = true;
                                            scores.push(tokenSearchResult.score);
                                        } else {
                                            obj[word] = 1;
                                            if (!this.options.matchAllTokens) {
                                                scores.push(1);
                                            }
                                        }
                                        this._log('Token: "' + word + '", score: ' + obj[word]);
                                        // tokenScores.push(obj)
                                    }

                                    if (hasMatchInText) {
                                        numTextMatches += 1;
                                    }
                                }

                                averageScore = scores[0];
                                var scoresLen = scores.length;
                                for (var _i2 = 1; _i2 < scoresLen; _i2 += 1) {
                                    averageScore += scores[_i2];
                                }
                                averageScore = averageScore / scoresLen;

                                this._log('Token score average:', averageScore);
                            }

                            var finalScore = mainSearchResult.score;
                            if (averageScore > -1) {
                                finalScore = (finalScore + averageScore) / 2;
                            }

                            this._log('Score average:', finalScore);

                            var checkTextMatches = this.options.tokenize && this.options.matchAllTokens ? numTextMatches >= tokenSearchers.length : true;

                            this._log('\nCheck Matches: ' + checkTextMatches);

                            // If a match is found, add the item to <rawResults>, including its score
                            if ((exists || mainSearchResult.isMatch) && checkTextMatches) {
                                // Check if the item already exists in our results
                                var existingResult = resultMap[index];
                                if (existingResult) {
                                    // Use the lowest score
                                    // existingResult.score, bitapResult.score
                                    existingResult.output.push({
                                        key: key,
                                        arrayIndex: arrayIndex,
                                        value: value,
                                        score: finalScore,
                                        matchedIndices: mainSearchResult.matchedIndices
                                    });
                                } else {
                                    // Add it to the raw result list
                                    resultMap[index] = {
                                        item: record,
                                        output: [{
                                            key: key,
                                            arrayIndex: arrayIndex,
                                            value: value,
                                            score: finalScore,
                                            matchedIndices: mainSearchResult.matchedIndices
                                        }]
                                    };

                                    results.push(resultMap[index]);
                                }
                            }
                        } else if (isArray(value)) {
                            for (var _i3 = 0, len = value.length; _i3 < len; _i3 += 1) {
                                this._analyze({
                                    key: key,
                                    arrayIndex: _i3,
                                    value: value[_i3],
                                    record: record,
                                    index: index
                                }, {
                                    resultMap: resultMap,
                                    results: results,
                                    tokenSearchers: tokenSearchers,
                                    fullSearcher: fullSearcher
                                });
                            }
                        }
                    }
                }, {
                    key: '_computeScore',
                    value: function _computeScore(weights, results) {
                        this._log('\n\nComputing score:\n');

                        for (var i = 0, len = results.length; i < len; i += 1) {
                            var output = results[i].output;
                            var scoreLen = output.length;

                            var totalScore = 0;
                            var bestScore = 1;

                            for (var j = 0; j < scoreLen; j += 1) {
                                var score = output[j].score;
                                var weight = weights ? weights[output[j].key].weight : 1;
                                var nScore = score * weight;

                                if (weight !== 1) {
                                    bestScore = Math.min(bestScore, nScore);
                                } else {
                                    output[j].nScore = nScore;
                                    totalScore += nScore;
                                }
                            }

                            results[i].score = bestScore === 1 ? totalScore / scoreLen : bestScore;

                            this._log(results[i]);
                        }
                    }
                }, {
                    key: '_sort',
                    value: function _sort(results) {
                        this._log('\n\nSorting....');
                        results.sort(this.options.sortFn);
                    }
                }, {
                    key: '_format',
                    value: function _format(results) {
                        var finalOutput = [];

                        this._log('\n\nOutput:\n\n', JSON.stringify(results));

                        var transformers = [];

                        if (this.options.includeMatches) {
                            transformers.push(function (result, data) {
                                var output = result.output;
                                data.matches = [];

                                for (var i = 0, len = output.length; i < len; i += 1) {
                                    var item = output[i];

                                    if (item.matchedIndices.length === 0) {
                                        continue;
                                    }

                                    var obj = {
                                        indices: item.matchedIndices,
                                        value: item.value
                                    };
                                    if (item.key) {
                                        obj.key = item.key;
                                    }
                                    if (item.hasOwnProperty('arrayIndex') && item.arrayIndex > -1) {
                                        obj.arrayIndex = item.arrayIndex;
                                    }
                                    data.matches.push(obj);
                                }
                            });
                        }

                        if (this.options.includeScore) {
                            transformers.push(function (result, data) {
                                data.score = result.score;
                            });
                        }

                        for (var i = 0, len = results.length; i < len; i += 1) {
                            var result = results[i];

                            if (this.options.id) {
                                result.item = this.options.getFn(result.item, this.options.id)[0];
                            }

                            if (!transformers.length) {
                                finalOutput.push(result.item);
                                continue;
                            }

                            var data = {
                                item: result.item
                            };

                            for (var j = 0, _len2 = transformers.length; j < _len2; j += 1) {
                                transformers[j](result, data);
                            }

                            finalOutput.push(data);
                        }

                        return finalOutput;
                    }
                }, {
                    key: '_log',
                    value: function _log() {
                        if (this.options.verbose) {
                            var _console;

                            (_console = console).log.apply(_console, arguments);
                        }
                    }
                }]);

                return Fuse;
            }();

            module.exports = Fuse;

            /***/ })
        /******/ ]);
});
//# sourceMappingURL=fuse.js.map
var AllInOneMenuTemplate = '<%var chunks = ["<div class=\'cont all-in-one-toc\'><h1 class=\'heading1\'>" + localization.TableOfContents + "</h1>"],    markup;function htmlTreeBuilder(items) {    if (items.length) {        chunks.push("<ul class=\'contents contents_all-in-one\'>");    }    items.forEach(function (item) {        if (!item.hidden) {            chunks.push("<li class=\'contents__item\'>");            chunks.push("<a href=\'");            chunks.push(item.url.match(/([\\w_]+\\.htm[l]?)/i)[1]);            chunks.push("\'>");            chunks.push(item.text);            chunks.push("</a>");            if (item.children) {                htmlTreeBuilder(item.children);            }            chunks.push("</li>");        }    });    if (items.length) {        chunks.push("</ul>");    }}htmlTreeBuilder(menu);chunks.push("</div>");markup = chunks.join("");%><%= markup %>';
var AsideTemplate = '<div class="aside ">    <div class="aside__inner">        <div class="aside__search search hidden-xs hidden-sm">            <div class="search__wrapper">                <input  class="search__field js_search_text" id="serch-text"                        placeholder="<%= localization.SearchLabel %>" type="text" />                <button class="search__btn js_search" id="search-button"></button>                <button class="search__clear is-hidden js_search_clear"></button>            </div>        </div>        <div class="selector-mobile js_selector_mobile_versions"></div>        <div class="selector-mobile js_selector_mobile_langs"></div>        <div class="aside__menu js_tabs_viewport">            <div class="js-menu-content">                <div class="js_menu"></div>            </div><!--            <div id="tab-names" class="b-tab__content js-tab-content">                <div class="b-index js_index_literals"></div>            </div>-->            <div class="js-search-content js_not_default">                <div class="search-results js_search_result_wrapper"></div>            </div>        </div>    </div></div>';
var BottomLinksTemplate = '<div class="bottom-links">    <!--<div class="feedback">        <div class="feedback__heading">Feedback</div>        <div class="feedback__text">            Was this article helpful?        </div>        <div>            <button class="button" type="button">Yes</button>            <button class="button" type="button">No</button>        </div>    </div>-->    <% if ((localization.ArticleIdText && localization.ArticleIdText.trim() && serviceInfo.id) ||            (localization.LastReviewText && localization.LastReviewText.trim() && serviceInfo.reviewDate)) { %>        <div class="bottom-links__service-info">            <% if (serviceInfo.id) { %>                <%= localization.ArticleIdText %> <%= serviceInfo.id + serviceInfo.separator %>            <% } %>            <% if (serviceInfo.reviewDate) { %>                <%= localization.LastReviewText %> <%= serviceInfo.reviewDate %>            <% } %>        </div>    <% } %>    <div class="bottom-links__link">        <%= homeBtnHtml %>    </div></div>';
var ContainerTemplate = '<div class="main js_container">    <header class="header-wrapper js_header_placeholder"></header>    <div class="content js_main">        <div class="container">            <div class="row">                <div class="col-lg-4 col-md-5 col-xs-20 menu-box js_aside_placeholder"></div>                <div class="col-md-1 hidden-xs hidden-sm js_prev_btn_placeholder"></div>                <div class="col-lg-14 col-md-13 col-xs-20 js_content"></div>                <div class="col-md-1 hidden-xs hidden-sm js_next_btn_placeholder"></div>                <div class="col-lg-16 col-md-15 col-xs-20 align-flex-end">                    <footer class="js_footer_placeholder"></footer>                </div>            </div>        </div>    </div>    <aside class="mobile-search-results is-hidden g-scrollable js_scrollable js_mobile_search_result js_mobile_aside"></aside></div><div class="tooltip js_tooltip" data-id="">    <div class="tooltip__content js_tooltip_content"></div></div><% if (localization.RequestConsent && localization.CookiePolicyText && localization.CookiePolicyButton) { %>    <div class="cookie-policy js_cookie_policy">        <div class="cookie-policy__inner">            <%= localization.CookiePolicyText %>        </div>        <div class="cookie-policy__btn-wrapper">            <button type="button" class="cookie-policy__btn js_cookie_policy_btn"><%= localization.CookiePolicyButton %></button>        </div>    </div><% } %>';
var CustomStylesTemplate = '<style>    html {        color: <%= customization.ContentColorMain %>;    }    a {        color: <%= customization.LinkColor %>    }    a:hover {        color: <%= customization.LinkColorHover %>    }    a:visited {        color: <%= customization.LinkColorVisited %>    }    .page-title {        color: <%= customization.TitleColor %>;    }    /*.tablename,    .picturename,    .examplebodytext,    .examplebodytextblue2015,    .bottom-links__service-info,    .search-results__total,    .footer {        color: <%= customization.ContentColorSecondary %>;    }*/    .attentionbody,    .attentionbodytextindent,    .attentioncontinue1,    .attentioncontinue2,    .attentioncontinue3 {        color: <%= customization.AttentionBlockColor %>;    }    .warningbody,    .warningbodytextindent,    .warningcontinue1,    .warningcontinue2,    .warningcontinue3 {        color: <%= customization.WarningBlockColor %>;        border-color: <%= customization.WarningBlockBorder %>;        background: <%= customization.WarningBlockBackground %>;    }    .example-wrapper {        background: <%= customization.ExampleBlockBackground %> !important;    }    .exampleheading,    .examplebodytext,    .exampleheadingblue2015,    .examplebodytextblue2015 {        color: <%= customization.ExampleBlockColor %>;    }    mark {        background: <%= customization.SearchHighlightBackground %>;        color: <%= customization.SearchHighlightColor %>;    }    /* icons */    <% if (customization.IconsX1) { %>        .logo {            background-image: url(<%= customization.IconsX1.Logo %>);        }        .top-bar__icon_print {            background-image: url(<%= customization.IconsX1.PrintGray %>);        }        .top-bar__icon_support {            background-image: url(<%= customization.IconsX1.SupportGray %>);        }        .top-bar__icon_feedback {            background-image: url(<%= customization.IconsX1.EmailGray %>);        }        .search__btn:after {            background-image: url(<%= customization.IconsX1.SearchGray %>);        }        .header__search-btn {            background-image: url(<%= customization.IconsX1.SearchGreen %>);        }        .contents__toggle {            background-image: url(<%= customization.IconsX1.Expand %>);        }        .contents__toggle.is-toggled {            background-image: url(<%= customization.IconsX1.Collapse %>);        }        .nav-btn__link-next {            background-image: url(<%= customization.IconsX1.PathNext %>);        }        .nav-btn__link-prev {            background-image: url(<%= customization.IconsX1.PathPrev %>);        }        .hyperlinktemplate,        .namedhyperlinktemplate {            background-image: url(<%= customization.IconsX1.Externallink %>);        }        a.popuponclicktemplate:after,        a.glossaryhtmllinktemplate:after,        a.popuponclickcontexthelptemplate:after,        .expandingblocktemplate,        .expandingblocktemplatemac {            background-image: url(<%= customization.IconsX1.Expandblock %>);        }        [dir="rtl"] a.popuponclicktemplate:after,        [dir="rtl"] a.glossaryhtmllinktemplate:after,        [dir="rtl"] a.popuponclickcontexthelptemplate:after,        [dir="rtl"] .expandingblocktemplate:after,        [dir="rtl"] .expandingblocktemplatemac:after {            background-image: url(<%= customization.IconsX1.ExpandblockRtl %>);        }        .expandingblockclose:after {            background-image: url(<%= customization.IconsX1.Expandblockclose %>);        }        .header__mobile-menu-btn {            background-image: url(<%= customization.IconsX1.ContentsMobile %>);        }        .header__mobile-menu-btn.is-active {            background-image: url(<%= customization.IconsX1.GoBackGreen %>);        }    <% } %>    <% if (customization.IconsX2) { %>        @media only screen and (-webkit-min-device-pixel-ratio: 1.5) {            .logo {                background-image: url(<%= customization.IconsX2.Logo %>);            }            .top-bar__icon_print {                background-image: url(<%= customization.IconsX2.PrintGray %>);            }            .top-bar__icon_support {                background-image: url(<%= customization.IconsX2.SupportGray %>);            }            .top-bar__icon_feedback {                background-image: url(<%= customization.IconsX2.EmailGray %>);            }            .search__btn:after {                background-image: url(<%= customization.IconsX2.SearchGray %>);            }            .header__search-btn {                background-image: url(<%= customization.IconsX2.SearchGreen %>);            }            .contents__toggle {                background-image: url(<%= customization.IconsX2.Expand %>);            }            .contents__toggle.is-toggled {                background-image: url(<%= customization.IconsX2.Collapse %>);            }            .nav-btn__link-next {                background-image: url(<%= customization.IconsX2.PathNext %>);            }            .nav-btn__link-prev {                background-image: url(<%= customization.IconsX2.PathPrev %>);            }            .hyperlinktemplate,            .namedhyperlinktemplate {                background-image: url(<%= customization.IconsX2.Externallink %>);            }            a.popuponclicktemplate:after,            a.glossaryhtmllinktemplate:after,            a.popuponclickcontexthelptemplate:after,            .expandingblocktemplate,            .expandingblocktemplatemac {                background-image: url(<%= customization.IconsX2.Expandblock %>);            }            [dir="rtl"] a.popuponclicktemplate:after,            [dir="rtl"] a.glossaryhtmllinktemplate:after,            [dir="rtl"] a.popuponclickcontexthelptemplate:after,            [dir="rtl"] .expandingblocktemplate:after,            [dir="rtl"] .expandingblocktemplatemac:after {                background-image: url(<%= customization.IconsX2.ExpandblockRtl %>);            }            .expandingblockclose:after {                background-image: url(<%= customization.IconsX2.Expandblockclose %>);            }            .header__mobile-menu-btn {                background-image: url(<%= customization.IconsX2.ContentsMobile %>);            }            .header__mobile-menu-btn.is-active {                background-image: url(<%= customization.IconsX2.GoBackGreen %>);            }        }    <% } %></style>';
var FooterTemplate = '<div class="footer footer_inner">    <!--<div class="container">-->        <div class="row">            <div class="col-xs-20">                <div class="footer__divider"></div>            </div>            <div class="col-md-10 col-xs-20">                <div class="footer__text">                    <%= localization.FooterCopyrightText %>                </div>            </div>            <div class="col-md-10 col-xs-20">                <div class="footer__links">                    <% if ( localization.FooterPrivacyPolicyLink && localization.FooterPrivacyPolicyLink.trim() &&                            localization.FooterPrivacyPolicyText && localization.FooterPrivacyPolicyText.trim() &&                            (!customization.FooterPrivacyPolicyLink || !customization.FooterPrivacyPolicyLink.Hidden)) { %>                        <span class="footer__links-item">                            <% if (customization.FooterPrivacyPolicyLink && customization.FooterPrivacyPolicyLink.Disabled) { %>                                <span class="footer__link">                                    <%= localization.FooterPrivacyPolicyText %>                                </span>                            <% } else { %>                                <a class="footer__link" target="blank" href="<%= localization.FooterPrivacyPolicyLink %>">                                    <%= localization.FooterPrivacyPolicyText %>                                </a>                            <% } %>                        </span>                    <% } %>                    <% if ( localization.FooterLegalText && localization.FooterLegalText.trim() &&                            (!customization.FooterLegalLink || !customization.FooterLegalLink.Hidden)) { %>                        <span class="footer__links-item">                            <% if (customization.FooterLegalLink && customization.FooterLegalLink.Disabled) { %>                                <span class="footer__link">                                    <%= localization.FooterLegalText %>                                </span>                            <% } else { %>                                <a href="javascript: void(0);" onclick="javascript: Cookiebot.renew()">                                    <%= localization.FooterLegalText %>                                </a>                            <% } %>                        </span>                    <% } %>                </div>            </div>        </div>    <!--</div>--></div>';
var HeaderTemplate = '<div class="header header_inner-help">    <div class="container">        <div class="row">            <div class="col-md-10 col-xs-20">                <button class="header__mobile-menu-btn js_burger" type="button"></button>                <div class="header__title hidden-xs hidden-sm">                    <% if ((!customization.HeaderLogoLink || !customization.HeaderLogoLink.Hidden) && localization.HeaderTitle) { %>                        <% if ((customization.HeaderLogoLink && customization.HeaderLogoLink.Disabled) || isOffline) { %>                    <span class="header__title-link">                        <% } else { %>                        <a href="<%= localization.HeaderLogoLink %>" class="header__title-link">                            <% } %>                            <% var index = localization.HeaderTitle.indexOf(" ") %>                            <%= localization.HeaderTitle.slice(0, index) %>                            <div class="header__title-big header__title-big_inner-help">                                <%= localization.HeaderTitle.slice(index, localization.HeaderTitle.length) %>                            </div>                            <%= (customization.HeaderLogoLink && customization.HeaderLogoLink.Disabled) ? "</span>" : "</a>" %>                            <% } %>                </div>                <div class="header__title header__title_product"><%= titlePreffix %> <%= title %></div>                <div class="header__search">                    <button class="header__search-btn js_mobile_search_button" type="button"></button>                    <input type="search" class="header__search-input is-hidden js_mobile_search_input"                           placeholder="<%= localization.SearchPlaceholder %>"/>                    <button class="header__clear-btn is-hidden js_mobile_search_clear" type="button"></button>                </div>            </div>            <div class="col-md-10 hidden-xs hidden-sm">                <div class="header__logo">                    <% if (!customization.FooterLogoLink || !customization.FooterLogoLink.Hidden) { %>                        <% if (customization.FooterLogoLink && customization.FooterLogoLink.Disabled) { %>                            <span class="logo"></span>                        <% } else { %>                            <a href="<%= localization.FooterLogoLink %>" class="logo"                               title="<%= localization.FooterLogoTitle %>"></a>                        <% } %>                    <% } %>                </div>            </div>        </div>    </div></div><div class="top-bar hidden-xs hidden-sm">    <div class="container">        <div class="row">            <div class="col-lg-5 col-md-6">                <div class="top-bar__product-box">    <a href="<%= localization.HeaderLogoLink %>" class="header__title-link">   <div class="top-bar__product js_product_logo"></div>        <h1 class="top-bar__title js_title">            <% if (titlePreffix && titlePreffix.trim() ) { %>                <span class="top-bar__title-small"><%= titlePreffix %></span>            <% } %>            <%= title %>        </h1></a></div>            </div>            <div class="col-lg-15 col-md-14">                <div class="top-bar__links-box">                    <div class="top-bar__links">                        <% if ( localization.PrintButtonText && localization.PrintButtonText.trim() && !customization.PrintButtonHidden) { %>                            <% if (isAllInOne) { %>                                <a class="top-bar__link top-bar__link_print" href="javascript:print();">                                    <span class="top-bar__icon top-bar__icon_print"></span><%= localization.PrintButtonText %>                                </a>                            <% } else { %>                                <div class="dropdown js_dropdown">                                    <a class="top-bar__link top-bar__link_print js_dropdown_btn" href="#">                                        <span class="top-bar__icon top-bar__icon_print"></span><%= localization.PrintButtonText %>                                        <span class="top-bar__triangle"></span>                                    </a>                                    <ul class="dropdown__list js_dropdown_list">                                        <li class="dropdown__item">                                            <a class="dropdown__link" href="javascript:print();">                                                <%= localization.PrintPageText %>                                            </a>                                        </li>                                        <li class="dropdown__item">                                            <a class="dropdown__link js_print_section" href=""                                               target="_blank" rel="noopener noreferrer">                                                <%= localization.PrintSectionText %>                                            </a>                                        </li>                                        <li class="dropdown__item">                                            <a class="dropdown__link js_print_all"                                               href="<%= isMacProject ? "pgs/" : "" %>all-in-one.htm"                                               target="_blank" rel="noopener noreferrer">                                                <%= localization.PrintAllText %>                                            </a>                                        </li>                                    </ul>                                </div>                            <% } %>                        <% } %>                        <% if ( localization.SupportButtonText && localization.SupportButtonText.trim() &&                                localization.SupportButtonLink && localization.SupportButtonLink.trim() &&                                (!customization.SupportButtonLink || !customization.SupportButtonLink.Hidden)) { %>                            <% if (customization.SupportButtonLink && customization.SupportButtonLink.Disabled) { %>                                <span class="top-bar__link top-bar__link_support">                                    <span class="top-bar__icon top-bar__icon_support"></span><%= localization.SupportButtonText %>                                </span>                            <% } else { %>                                <a class="top-bar__link top-bar__link_support"                                   href="<%= localization.SupportButtonLink %>" target="blank">                                    <span class="top-bar__icon top-bar__icon_support"></span><%= localization.SupportButtonText %>                                </a>                            <% } %>                        <% } %>                        <% if ( localization.SendLinkButtonText && localization.SendLinkButtonText.trim() &&                                (!customization.SendLinkButton || !customization.SendLinkButton.Hidden)) { %>                            <% if (customization.SendLinkButton && customization.SendLinkButton.Disabled) { %>                                <span class="top-bar__link top-bar__link_feedback">                                    <span class="top-bar__icon top-bar__icon_feedback"></span><%= localization.SendLinkButtonText %>                                </span>                            <% } else { %>                                <a class="top-bar__link top-bar__link_feedback js_feedback_link" href="">                                    <span class="top-bar__icon top-bar__icon_feedback"></span><%= localization.SendLinkButtonText %>                                </a>                            <% } %>                        <% } %>                        <% if (pdfOptions && pdfOptions.pdfName && pdfOptions.pdfName.trim()){ %>                            <% if (localization.SaveAsPdfButtonText && localization.SaveAsPdfButtonText.trim() &&                                    (!customization.SaveAsPdfButton || !customization.SaveAsPdfButton.Hidden)) { %>                                <% if (customization.SaveAsPdfButton && customization.SaveAsPdfButton.Disabled) { %>                                    <span class="top-bar__link top-bar__link_pdf">                                    <span class="top-bar__icon top-bar__icon_save-as-pdf"></span><%= localization.SaveAsPdfButtonText %>                                </span>                                <% } else { %>                                    <a class="top-bar__link top-bar__link_pdf js_pdf_link" href="<%=pdfOptions.pdfName.trim()%>">                                        <span class="top-bar__icon top-bar__icon_save-as-pdf"></span><%= localization.SaveAsPdfButtonText %>                                    </a>                                <% } %>                                                            <% } %>                                                    <% } %>												<% if (localization.OnlineHelpButtonText && localization.OnlineHelpButtonText.trim() && localization.OnlineHelpButtonLink && localization.OnlineHelpButtonLink.trim()) { %>							<span class="top-bar__link"><a class="top-bar__link" href="<%= localization.OnlineHelpButtonLink %>"><%= localization.OnlineHelpButtonText %></a></span>						<% } %>						                    </div>                    <% if (!customization.HideLangsSelector) { %>                        <div class="top-bar__langs">                            <span class="top-bar__versions js_header_versions_list"></span>                            <span class="js_header_lang_list"></span>                        </div>                    <% } %>                </div>            </div>        </div>    </div></div>';
var IndexLiteralsTemplate = '<ul class="b-index__list"><%    var result,        html = [];    for (var i= 0, item; i < index.length; i++ ) {        item = index[i];        if ( item.nested) {            html.push( "<li class=\'b-index__item js_menu_item b-index__item--nested\'>" );        } else {            html.push( "<li class=\'b-index__item js_menu_item\'>" );        }        if ( item["class"] === "indexlink" ) {            html.push( "<a class=\'b-index__link js_menu_link\' href=\'");            html.push( item.href );            html.push( "\'>" );            html.push( item.text );            html.push( "</a>" )        } else if ( item["class"] === "indexheading" ) {            html.push( "<span class=\'b-index__header\'>" );            html.push( item.text );            html.push( "</span>" )        } else {            html.push( "<span class=\'b-index__subheader\'>" );            html.push( item.text );            html.push( "</span>" )        }        html.push( "</li>" );    }    result = html.join("");%><%= result %></ul>';
var LangListTemplate = '<div class="dropdown dropdown_right js_dropdown">    <a class="dropdown__btn js_dropdown_btn">        <%= currentLang.name %>        <span class="dropdown__arrow"></span>    </a>    <ul class="dropdown__list js_dropdown_list">        <% _.each( langs, function( lang ) {%>            <% var dir = lang.dirname || lang.id %>            <% if (lang.id !== currentLang.id ) { %>                <li class="dropdown__item">                    <% if ( isMac ) { %>                        <a class="dropdown__link js_lang_item" data-lang-id="<%= dir %>" href="../../<%= dir %>/pgs">                            <%= lang.name %>                        </a>                    <% } else { %>                        <a class="dropdown__link js_lang_item" data-lang-id="<%= dir %>" href="../<%= dir %>">                            <%= lang.name %>                        </a>                    <% } %>                </li>            <% } %>        <% }) %>    </ul></div>';
var MenuTemplate = '<%var chunks = [],    isMac = isMacProject,    markup;function htmlTreeBuilder( items, opened ) {    if (items.length) {        chunks.push( "<ul class=\'contents js_contents_level\'" );        chunks.push( !opened ? " style=\'display:none\' ": "" );        chunks.push( ">" );    }    for (var i = 0, item; i < items.length; i++) {        item = items[i];        if ( !item.hidden ) {            if ( item.isActive ) {                chunks.push( "<li class=\'contents__item js_menu_item is-active\'>" );            } else {                chunks.push( "<li class=\'contents__item js_menu_item\'>" );            }            if ( item.children ) {                chunks.push( "<button class=\'contents__toggle js_contents_toggle" );                chunks.push( item.childIsActive || item.isActive ? " is-toggled" : "" );                chunks.push( "\' href=\'#\'></button>" );            }            chunks.push( "<a class=\'contents__item-link js_menu_link\' href=\'" );            chunks.push( (isMac ? "index.htm#" : "") + item.url );            chunks.push( "\'>" );            chunks.push( item.text );            chunks.push( "</a>" );            if ( item.children ) {                htmlTreeBuilder( item.children, item.childIsActive || item.isActive );            }        }    }    if (items.length) {        chunks.push( "</ul>" );    }}htmlTreeBuilder( menu, true );markup = chunks.join("");%><%= markup %>';
var MobileSearchResultsTemplate = '<%if (!isInit){%>    <div class="mobile-search-results__total">        <% if ( resultsCount > 0) { %>            <%= localization.FoundTextBegin %> <%= resultsCount %> <%= localization.FoundTextEnd %>        <% } else { %>            <%= localization.NothingFoundText %>        <%}%>    </div>    <div class="mobile-search-results__content">        <%if ( found ){%>            <ul class="mobile-search-results__list">                <%_.each(results, function( page ){function escapeHTML(html) {  return html.replace(/&/g, "&amp;").replace(/</g,"&lt;").replace(/>/g, "&gt;").replace(/&lt;(\\/)?mark&gt;/g,"<$1mark>");}%>                    <li class="mobile-search-result">                        <% var link = isMacProject ? "index.htm#" + page.link : page.link %>                        <a class="mobile-search-result__title" href="<%= link %>"><%= page.title %></a>                        <%_.each( page.texts, function( text ){var etext=escapeHTML(text);%>                            <p class="mobile-search-result__content"><%= etext %></p>                        <%})%>                    </li>                <%})%>            </ul>        <%}%>    </div><%}%>';
var NextLinkTemplate = '<div class="nav-btn">    <% if (nextPage && nextPage.url) { %>        <% var link = isMacProject ? "index.htm#" + nextPage.url : nextPage.url %>        <a class="nav-btn__link-next js_next_link"           href="<%= link %>"           title="<%= nextPage.text %>"></a>    <% } else { %>        <span class="nav-btn__link-next-inactive"></span>    <% } %></div>';
var PageTitleTemplate = '<div class="page-title">    <div class="page-title__text">        <%= titleHtml %>    </div></div>';
var PrevLinkTemplate = '<div class="nav-btn">    <% if (prevPage && prevPage.url) { %>        <% var link = isMacProject ? "index.htm#" + prevPage.url : prevPage.url %>        <a class="nav-btn__link-prev js_prev_link"           href="<%= link %>"           title="<%= prevPage.text %>"></a>    <% } else { %>        <span class="nav-btn__link-prev-inactive"></span>    <% } %></div>';
var SearchResultsTemplate = '<%if (!isInit){%>    <div class="search-results__header">        <div class="search-results__total">            <%= localization.FoundTextBegin %> <%= resultsCount %> <%= localization.FoundTextEnd %>        </div>    </div>    <%if ( found ){%>        <div class="search-results__content">            <ul class="search-results__list">                <%_.each(results, function( page ){ function escapeHTML(html) {  return html.replace(/&/g, "&amp;").replace(/</g,"&lt;").replace(/>/g, "&gt;").replace(/&lt;(\\/)?mark&gt;/g,"<$1mark>");}%>                    <li class="search-result">                        <% var link = isMacProject ? "index.htm#" + page.link : page.link %>                        <a class="search-result__title" href="<%= link %>"><%= page.title %></a>                        <%_.each( page.texts, function( text ){ var etext=escapeHTML(text); %>                            <p class="search-result__content"><%= etext %></p>                        <%})%>                    </li>                <%})%>            </ul>        </div>    <%}%><%}%>';
var VersionsTemplate = '<div class="dropdown dropdown_right js_dropdown">    <a class="dropdown__btn js_dropdown_btn">        <%= currentVersion.label %>        <span class="dropdown__arrow"></span>    </a>    <ul class="dropdown__list js_dropdown_list">        <% _.each(versions, function(version)  {%>            <% if (version.url !== currentVersion.url) { %>                <li class="dropdown__item">                    <a class="dropdown__link js_version_item"                       data-version-url="<%= version.url %>" href="../../<%= version.url %>">                        <%= version.label %>                    </a>                </li>            <% } %>        <% }) %>    </ul></div>';
var LangsLocalization = {"en-US":"English","id-ID":"Bahasa Indonesia","cs-CZ":"Čeština &#x202A;(Česká republika)","da-DK":"Dansk &#x202A;(Danmark)","de-DE":"Deutsch","et-EE":"Eesti","es-ES":"Español &#x202A;(España)","es-MX":"Español &#x202A;(México)","fr-CA":"Français &#x202A;(Canadien)","fr-FR":"Français","it-IT":"Italiano","lv-LV":"Latviešu","lt-LT":"Lietuvių","hu-HU":"Magyar &#x202A;(Magyarország)","nl-NL":"Nederlands &#x202A;(Nederland)","nb-NO":"Norsk, bokmål &#x202A;(Norge)","pl-PL":"Polski &#x202A;(Polska)","pt-BR":"Português &#x202A;(Brasil)","pt-PT":"Português &#x202A;(Portugal)","ro-RO":"Română &#x202A;(România)","sr-Latn":"Srpski","fi-FI":"Suomi &#x202A;(Suomi)","sv-SE":"Svenska &#x202A;(Sverige)","vi-VN":"Tiếng Việt &#x202A;(Việt Nam)","tr-TR":"Türkçe &#x202A;(Türkiye)","el-GR":"Ελληνικά &#x202A;(Ελλάδα)","bg-BG":"Български","kk-KZ":"Қазақша","ru-RU":"Русский","sr-Cyrl":"Српски","uk-UA":"Українська","ar-AE":"العربية (الإمارات العربية المتحدة&#x202B;)","fa-IR":"فارسى (ایران&#x202B;)","hi-IN":"हिंदी &#x202A;(भारत)","th-TH":"ไทย &#x202A;(ไทย)","ko-KR":"한국어 &#x202A;(대한민국)","ja-JP":"日本語&#x202A;(日本)","zh-Hans":"简体中文","zh-Hant":"繁體中文","zh-Hant-TW":"繁體中文(台灣)","zh-Hant-HK":"繁體中文(香港)"};
var Page=new Array();Page[0]=new Array("Kaspersky Neuromorphic Platform (далее также платформа) &#8211; это программно-аппаратная платформа, предназначенная для эмуляции импульсных нейронных сетей и поддержки их выполнения на разных вычислителях.","С помощью Kaspersky Neuromorphic Platform вы можете: ","Создавать и обучать нейронные сети на различных типах входных данных&nbsp;(например, телеметрия, события, изображения, 3D-данные и звуковые и тактильные данные). ","Оптимизировать структуру и гиперпараметры загруженных нейронных сетей.","Проводить прикладные исследования в области классификации входных данных и других областях применения импульсных нейронных сетей. ","Разрабатывать новые топологии нейронных сетей&nbsp;(например, импульсные аналоги сверточных нейронных сетей, предполагающих свертку по пространству и времени). ","Разрабатывать новые модели синаптической пластичности.","Реализовывать новые модели нейронов.","Реализовывать прикладные решения на основе нейроморфных импульсных нейронных сетей и искусственного интеллекта в области робототехнических манипуляторов, интернета вещей, беспилотных систем, человеко-машинного взаимодействия, носимых устройств и оптимизационного планирования.","Реализовывать прикладные решения на устройствах с пониженным энергопотреблением или с использованием нейроморфных процессоров&nbsp;(англ. Neural Processing Unit, NPU).","Для решения вышеперечисленных задач вы можете использовать языки C++ и Python. Платформа поддерживает CPU. Фреймворк Kaspersky Neuromorphic Platform можно также использовать как бекенд для PyNN API.","О Kaspersky Neuromorphic Platform","232786.htm");
Page[1]=new Array("Комплект поставки Kaspersky Neuromorphic Platform зависит от области применения платформы. Kaspersky Neuromorphic Platform может быть использован для работы с нейронными сетями (например, для реализации новых моделей нейронов), а также совместной разработки платформы.","Комплект поставки Kaspersky Neuromorphic Platform для разработки прикладных решений","В комплект поставки входят следующие пакеты:","deb-пакеты с бинарным кодом бекендов, включая код бекенда для нейроморфного процессора Алтай-2.","deb-пакет с бинарным кодом фреймворка для C++, содержащий:","Бинарный код библиотек платформы.","Файл с условиями использования платформы LICENSE.txt на английском языке.","Файл с информацией о платформе README.md на английском языке.","dev-deb пакет для deb-пакета с бинарным кодом на C++. В dev-deb пакет входят следующие файлы:","Заголовочные файлы.","Модули CMake для сборки платформы на C++.","Статические версии библиотек платформы.","deb-пакет с Python-фреймворком.","deb-пакет со следующими файлами: ","Документацией по использованию платформы.","Справочным руководством по API.","Комплект поставки Kaspersky Neuromorphic Platform для разработки прикладных решений на языке программирования Python","В комплект поставки Kaspersky Neuromorphic Platform входят следующие файлы:","deb-пакет с Python-фреймворком.","whl-пакет для установки с платформы PyPI с помощью менеджера пакетов pip. ","Комплект поставки Kaspersky Neuromorphic Platform с исходным кодом платформы","Kaspersky Neuromorphic Platform поставляется в виде архива, в который входят следующие файлы:","Исходный код платформы.","Документация по использованию платформы.","Справочное руководство по API. ","Файл с информацией о платформе README.md на английском языке.","Файл с условиями использования платформы LICENSE.txt на английском языке.","Файл с информацией о платформе (Release Notes).","Файл с информацией о стороннем коде NOTICES.txt на английском языке.","Файл с правилами участников репозитория CONTRIBUTING.md на английском языке.","Файл с условиями участия в репозитории Contributor License Agreement.docx на английском языке.","Вы можете получить файлы архива путем клонирования репозитория платформы.","Комплект поставки","232787.htm");
Page[2]=new Array("Для функционирования Kaspersky Neuromorphic Platform компьютер должен удовлетворять следующим минимальным требованиям.","Список поддерживаемых процессоров:","центральный процессор: совместимый процессор Intel Core i5 и выше;","нейроморфный процессор Алтай-2.","Минимальные аппаратные требования:","1 ядро;","8 ГБ оперативной памяти;","10 ГБ свободного пространства на жесткой диске. ","Процессор должен поддерживать следующие расширения:","Advanced Vector Extensions (avx);","Advanced Vector Extensions 2 (avx2).","Поддерживаемые операционные системы:","Debian GNU/Linux 12.5 и выше;","Ubuntu 22.04 LTS и выше.","Вы можете использовать устройство под управлением любой другой операционной системы из семейства Linux, если дистрибутив операционной системы содержит библиотеку Boost версии 1.80 и выше.","Для работы c платформой на устройстве должны быть установлено следующее программное обеспечение: ","Python 3.8;","TensorFlow 2.4.1 и выше;","Keras 2.3.1 и выше;","NumPy 2.4.1 и выше.","Версия библиотеки NumPy должна соответствовать версии библиотеки TensorFlow. ","Аппаратные и программные требования","232788.htm");
Page[3]=new Array("Бекенд Kaspersky Neuromorphic Platform предоставляет стандартизированный интерфейс для работы с вычислителем. Бекенд платформы позволяет запускать эмуляцию работы импульсной нейронной сети на различных вычислительных устройствах.","Устройство или код, непосредственно выполняющий сеть.","Физически бекенд платформы разделяется на библиотеки и компилируется в несколько независимых друг от друга бекендов. Каждый такой бекенд позволяет выполнять эмуляцию на разных типах устройств.","Бекенд Kaspersky Neuromorphic Platform выполняет следующие функции:","Запускает импульсные нейронные сети на различных вычислителях.","Выполняет статические проверки корректности построения импульсной нейронной сети.","Предоставляет общую кодовую базу для реализации вычислителей.","О бекенде Kaspersky Neuromorphic Platform","235051.htm");
Page[4]=new Array("Kaspersky Neuromorphic Platform обеспечивает обмен данными между объектами библиотеки поддержки бекендов, а также между объектами платформы и объектами внешней среды. Обмен данными между объектами библиотеки поддержки бекенда Kaspersky Neuromorphic Platform осуществляется через сообщения.","Обмен данными между популяциями и проекциями","Нейроны популяции генерируют сообщения, содержащие спайки, на которые может быть подписано более одной проекции. Спайки популяций передаются проекциям синапсов.","Контейнер, содержащий набор синапсов одного типа, соединяющих нейроны пресинаптической и постсинаптической популяций. ","Кратковременный потенциал действия, сгенерированный нейроном популяции в результате изменения атрибутов нейрона.","Узел нейронной сети, имеющий набор атрибутов. На основе атрибутов нейрона и последовательности входных сигналов происходит вычисление некоторой математической функции, результатом которого является синаптическое воздействие или его отсутствие.","После получения спайков бекенд вычисляет синаптические воздействия. Проекции отправляют сообщения со значениями синаптических воздействий постсинаптическим популяциям нейронов. ","Значение, формируемое синапсом и предназначенное для изменения значений атрибутов нейронов.","Обмен данными между бекендом и внешней средой","Бекенд принимает данные от внешней среды в виде спайков. Спайки могут быть переданы в бекенд в синхронном режиме&nbsp;(по запросу пользователя). Для получения спайков в синхронном режиме вызывается блокирующий метод источника данных. ","Сообщения от бекенда во внешнюю среду могут передаваться подписчикам по сигналу бекенда или в синхронном режиме по запросу пользователя.","Обмен данными между нейронной сетью и внешней средой","Из внешней среды в нейронную сеть данные поступают через каналы ввода, которые преобразуют эти данные в спайки. ","Во внешнюю среду нейронная сеть передает данные через каналы вывода, подключенные к указанным популяциям и преобразующие спайки в данные. Передача данных, например потенциала мембраны, возможна также через подсистему мониторинга.  ","Обмен данными в Kaspersky Neuromorphic Platform","244427.htm");
Page[5]=new Array("В Kaspersky Neuromorphic Platform поддерживаются следующие типы бекендов:","Бекенды для CPU:","Однопоточный бекенд для CPU. Бекенд работает только в одном потоке. Вы можете использовать однопоточный бекенд для отладки платформы и работы нейронной сети. ","Многопоточный бекенд для CPU. Бекенд работает в нескольких потоках одновременно, что обеспечивает повышение скорости расчета нейронной сети. Вы можете использовать многопоточный бекенд для эмуляции импульсной нейронной сети.","Бекенды для нейроморфного процессора Алтай-2:","SNN. Бекенд используется для эмуляции импульсных нейронных сетей.  ","На рисунке ниже представлена схема взаимодействия компонентов платформы при использовании бекенда SNN. Импульсная нейронная сеть создается с помощью компонентов платформы и обучается методом синаптической пластичности. Далее нейронная сеть размещается с помощью утилиты placer на нейроморфном процессоре Алтай-2. Вы можете запускать исполнение импульсных нейронных сетей как на программном эмуляторе в составе бекенда SNN, так и непосредственно на аппаратном обеспечении нейроморфного процессора Алтай-2.","Схема взаимодействия платформы с бекендом SNN ","ANN2SNN. Бекенд используется для эмуляции нейронных сетей, преобразованных в импульсные. Обучение нейронной сети производится обратным распространением ошибки с квантованием весов. ","На рисунке ниже представлена схема взаимодействия компонентов платформы при использовании бекенда ANN2SNN. Нейронная сеть создается с помощью тернарных слоев библиотеки ANN2SNN, реализованной как часть Python-фреймворка, и обучается путем обратного распространения ошибки с квантованием весов. Далее нейронная сеть размещается с помощью утилиты placer на нейроморфном процессоре Алтай-2. Размещенная нейронная сеть может быть исполнена как на программном эмуляторе в составе бекенда ANN2SNN, так и непосредственно на аппаратном обеспечении нейроморфного процессора Алтай-2.","Схема взаимодействия компонентов платформы в случае использования библиотеки ANN2SNN","Поддерживаемые бекенды","244808.htm");
Page[6]=new Array("Работа бекенда для CPU основывается на обмене сообщениями между проекциями и популяциями. Объекты библиотеки поддержки бекендов также могут обмениваться сообщениями с объектами внешней среды. Для обмена сообщений бекенд предоставляет шину сообщений MessageBus. ","Для инициализации бекенда требуется предварительно загрузить проекции, содержащие синапсы, и популяции, содержащие нейроны. В процессе инициализации бекенда загруженные проекции синапсов подписываются на спайки от связанных популяций или каналов ввода, а загруженные популяции подписываются на синаптические воздействия от связанных проекций. ","Значение, формируемое синапсом и предназначенное для изменения значений атрибутов нейронов.","Кратковременный потенциал действия, сгенерированный нейроном популяции в результате изменения атрибутов нейрона.","Узел нейронной сети, имеющий набор атрибутов. На основе атрибутов нейрона и последовательности входных сигналов происходит вычисление некоторой математической функции, результатом которого является синаптическое воздействие или его отсутствие.","Место соединения нейронов, основная функция которого&nbsp;&#8211; передача сигнала от одного нейрона к другому. Каждый нейрон может быть связан с множеством синапсов.","Цикл работы бекенда состоит из следующих этапов:","Каналы ввода отправляют данные в шину сообщений MessageBus.","Бекенд вычисляет функцию популяции нейронов.","Популяции нейронов отправляют спайки с результатами вычислений в шину сообщений MessageBus с помощью точек подключения MessageEndpoint.","Шина сообщений MessageBus отправляет данные, полученные от популяций, в проекции или каналы вывода.","Бекенд вычисляет функцию проекции синапсов. ","Проекции синапсов отправляют синаптические воздействия с результатами вычислений в шину сообщений MessageBus с помощью точек подключения MessageEndpoint.","Шина сообщений MessageBus отправляет данные, полученные от проекций, в связанные популяции нейронов.","О работе бекенда для CPU","272835.htm");
Page[7]=new Array("Нейроморфный процессор Алтай-2&nbsp;&#8211; это сверхбольшая интегральная схема (СБИС) с вычислительной архитектурой, которая имитирует работу биологических сенсорных и нервных систем. Нейроморфный процессор Алтай-2 предназначен для исполнения (инференса) импульсных нейронных сетей. В архитектуре процессора Алтай-2 реализован принцип \"вычисления рядом с памятью\", позволяющий избежать характерных для архитектуры фон Неймана избыточных энергозатрат на передачу данных между блоком памяти и вычислительным ядром. Нейроморфный процессор Алтай-2 обладает низким энергопотреблением. ","Элементарной единицей импульсной нейронной сети является нейрон, у которого есть набор входных сигнальных линий&nbsp;(дендриты) и выходной сигнальной линией&nbsp;(аксон). Состояние нейрона описывается нейронным потенциалом. Если в процессе исполнения импульсной нейронной сети потенциал нейрона превышает заданный порог, то нейрон испускает по своему аксону спайк и отправляет его в заданные целевые нейроны. Спайк может быть как положительным, так и отрицательным. При моделировании нейрона также учитывается механизм утечки, предназначенный для релаксации нейронного потенциала. ","Нейроны импульсной нейронной сети связаны друг с другом посредством синапсов. Каждому синапсу может быть присвоено значение веса, на которое изменяется потенциал нейрона при получении спайка от нейрона, связанного с этим синапсом.","Основной структурной единицей архитектуры нейроморфного процессора Алтай-2 является ядро. Ядро объединяет в себе группу нейронов и память для хранения их параметров. Функцию нейронов в ядре выполняет конечный автомат, который моделирует поведение нейронов. ","Синхронизация работы всех нейронов импульсной нейронной сети осуществляется с помощью сигнала тик, после подачи которого потенциалы всех нейронов последовательно обновляются всеми ядрами и при необходимости испускают спайки. За тик ядро выполняет 262&nbsp;144 синаптических операций. При этом частота тика не превышает 2000 Гц. Одна синаптическая операция выполняется за один такт синхросигнала с частотой 600 МГц.","Квант времени, за который выполняется вычисление функции всех нейронов в ядре. За время тика все спайки, сгенерированные ядрами или поступившие на входные линии нейронов, доставляются до целевых нейронов.","Нейроморфный процессор Алтай-2 имеет регулярную структуру ядер в виде прямоугольной сети, в которой каждое ядро непосредственно связано со своими 4 соседями. Для передачи спайков между нейронами разных ядер реализован механизм маршрутизации таких сигналов по сети ядер. Соответственно, при выполнении функции нейрона к потенциалу моделируемого нейрона могут быть добавлены значения нейронных потенциалов, отправленных нейронами из других ядер.","В нейроморфном процессоре Алтай-2 предусмотрена возможность масштабирования за счет увеличения количества СБИС. По вопросам масштабирования вам нужно обратиться к специалистам \"Лаборатории Касперского\".","В таблице ниже приведены общие сведения о нейроморфном процессоре Алтай-2 и его составляющих: ","Общие сведения о процессоре Алтай-2","Свойство","Ограничение","СБИС","Площадь кристалла","6 кв. мм","Технология изготовления","28 нм","Максимальное количество нейроморфных ядер","256","Количество нейроморфных ядер в прототипе","16","Максимальное количество моделируемых нейронов","131&nbsp;072","Количество моделируемых нейронов в прототипе","8192","Ядро","Количество моделируемых нейронов","512","Количество входных линий","512","Диапазон значений потенциала нейрона","От -32&nbsp;768 до 32&nbsp;767 ","Потенциал нейрона хранится в виде 16-битной величины.","Диапазон значений веса синапса","Вес синапса хранится в виде 8-битовой беззнаковой величины.","Вариативность весов на нейрон","4","Количество синаптических операций","9","Каждая синаптическая связь между моделируемым нейроном и входной линией характеризуется одной из девяти синаптических операций, выбором которых можно управлять воздействием этой синаптической связи на нейрон. Возможны следующие синаптические операции:","Пустая операция.","Во время этой операции не происходит никаких изменений.","Прибавить вес N, где N &#8211; это номер веса в диапазоне от 1 до 4.","Вычесть вес N, где N &#8211; это номер веса в диапазоне от 1 до 4.","Количество синаптических операций за тик","262&nbsp;144","Частота тика","Не более 2000 Гц","Энергопотребление ядра","Не более 4 мВт","Объем памяти ядра","88&nbsp;КБ","Объем памяти ядра для хранения параметров нейрона","16 КБ","О нейроморфном процессоре Алтай-2","272837.htm");
Page[8]=new Array("Kaspersky Neuromorphic Platform включает в себя следующие компоненты:","Бекенд. Запускает эмуляцию работы импульсной нейронной сети на различных вычислительных устройствах&nbsp;(например, центральных процессорах).","Фреймворк для C++. Обеспечивает управление бекендами.  ","Python-фреймворк. Обеспечивает управление бекендами. ","PyNN Module. Обеспечивает интерфейс к Python-фреймворку.","На рисунке ниже представлена схема взаимодействия компонентов Kaspersky Neuromorphic Platform.","Архитектура Kaspersky Neuromorphic Platform","Архитектура Kaspersky Neuromorphic Platform","232883.htm");
Page[9]=new Array("Этот раздел содержит описание компонентов для использования Kaspersky Neuromorphic Platform на языке программирования C++.","Компоненты платформы для использования на C++","272175.htm");
Page[10]=new Array("Для реализации бекендов вы можете использовать следующие библиотеки платформы на языке программирования C++:","Библиотека поддержки бекендов. Реализует основные объекты и шаблоны вычислителя.","Библиотека устройств. Предоставляет набор интерфейсов для взаимодействия с физическими устройствами.","Библиотека характеристик нейронов. Содержит наборы свойств и типизацию популяций моделей нейронов.","Узел нейронной сети, имеющий набор атрибутов. На основе атрибутов нейрона и последовательности входных сигналов происходит вычисление некоторой математической функции, результатом которого является синаптическое воздействие или его отсутствие.","Библиотека характеристик синапсов. Содержит набор классов, определяющих поведение синапса.","Место соединения нейронов, основная функция которого&nbsp;&#8211; передача сигнала от одного нейрона к другому. Каждый нейрон может быть связан с множеством синапсов.","Библиотека шаблонов. Предоставляет шаблоны общего назначения.","На рисунке ниже представлена схема взаимодействия библиотек платформы.","В Kaspersky Neuromorphic Platform вычислитель хранит в себе интерфейс устройства, а также экземпляры объектов библиотеки поддержки бекендов (например, проекции и популяции), необходимые для выполнения прикладных задач на вычислителе. Шаблоны популяций и проекций нейронной сети должны быть специализированы типами нейронов и синапсов, описанными в библиотеках характеристик нейронов и синапсов. ","Контейнер, содержащий набор нейронов и их функцию в соответствии с типом нейронов.","Контейнер, содержащий набор синапсов одного типа, соединяющих нейроны пресинаптической и постсинаптической популяций. ","Взаимодействие библиотек для C++ в Kaspersky Neuromorphic Platform","Компоненты бекенда платформы для C++","243537.htm");
Page[11]=new Array("В библиотеке поддержки бекендов реализуются интерфейсы к объектам, необходимым для работы бекендов и выполнения прикладных задач. Библиотека содержит пространство имен core.","Таблица ниже содержит описание объектов библиотеки.","Объект","Описание","Backend","Базовый класс для конкретных реализаций бекендов.","BaseData","Структура, определяющая набор базовых данных.","continuously_uid_generator","Класс, реализующий генератор уникальных идентификаторов.","Device","Базовый класс устройств.","MessageBus","Класс, реализующий интерфейс к шине сообщений.","MessageEndpoint","Класс, реализующий интерфейс к точке подключения.","Messaging","Пространство имен, определяющее интерфейсы к сообщениям.","Population","Шаблонный класс, реализующий контейнер нейронов одной модели.","Узел нейронной сети, имеющий набор атрибутов. На основе атрибутов нейрона и последовательности входных сигналов происходит вычисление некоторой математической функции, результатом которого является синаптическое воздействие или его отсутствие.","Projection","Шаблонный класс, реализующий контейнер синапсов одного типа.","Место соединения нейронов, основная функция которого&nbsp;&#8211; передача сигнала от одного нейрона к другому. Каждый нейрон может быть связан с множеством синапсов.","Subscription","Класс, реализующий подписку на сообщения.","synapse_access","Пространство имен, реализующее соединения и интерфейс для доступа к их реестру.","TagMap","Класс, реализующий интерфейс к словарю тегов.","UID","Класс, определяющий уникальные идентификаторы.","uid_hash","Функция, реализующая хеширование UID.","На рисунке ниже представлена схема взаимодействия объектов библиотеки.","В Kaspersky Neuromorphic Platform структура BaseData содержит базовую информацию об объекте и включает в себя UID и TagMap. Структура BaseData используется в классах Projection, Population и Backend, соответственно каждый из них обладает идентификатором UID и набором тегов и их значениями TagMap.","Классы Population и Projection принимают и отправляют сообщения из пространства имен Messaging, которые содержат в своем заголовке UID отправителя.  ","Класс Backend содержит классы Device и MessageBus. Шина сообщений MessageBus создает точки подключения MessageEndpoint и взаимодействует с ними в процессе обмена сообщениями. Каждая точка подключения MessageEndpoint содержит контейнер подписок класса Subscription.","Схема взаимодействия объектов библиотеки поддержки бекендов","Библиотека поддержки бекендов","235738.htm");
Page[12]=new Array("Объект класса UID является частью структуры BaseData и представляет собой идентификатор объекта. Идентификатор является уникальным.","Класс UID","247366.htm");
Page[13]=new Array("Функция uid_hash реализует функцию хеширования идентификаторов объектов.","Функция uid_hash","256488.htm");
Page[14]=new Array("Класс continuously_uid_generator реализует генератор уникальных идентификаторов&nbsp;(англ. UID). Идентификатор отображается в виде строки, разбитой на группы с помощью дефисов, и представляет собой 128-битное число. Идентификаторы генерируются последовательно с шагом, равным 1.","Вы можете использовать класс continuously_uid_generator для отладки платформы и исполнения нейронной сети.","Класс continuously_uid_generator","248929.htm");
Page[15]=new Array("Объект класса TagMap является составляющей частью структуры BaseData и представляет собой ассоциативный массив, включающий ноль или более записей типа \"ключ-значение тега\", где ключом является имя тега. Имена тегов имеют строковый тип и являются уникальными. ","Класс TagMap","244933.htm");
Page[16]=new Array("Структура BaseData определяет набор базовых данных, общих для нескольких объектов библиотеки поддержки бекендов и объектов фреймворка. Например, базовые данные BaseData используются в объектах классов Backend, Device, Population и Projection.","Базовые данные BaseData включают в себя следующее:","uid_&nbsp;&#8211; уникальный идентификатор объекта.","tags_&nbsp;&#8211; ассоциативный массив TagMap, состоящий из тегов, используемых объектом, и их значений.","Структура BaseData","248867.htm");
Page[17]=new Array("Шаблонный класс Population реализует контейнер, который содержит нейроны, принадлежащие одной модели. При использовании шаблона класса Population вы можете указать один из типов нейронов, поддерживаемых библиотекой характеристик нейронов.","Узел нейронной сети, имеющий набор атрибутов. На основе атрибутов нейрона и последовательности входных сигналов происходит вычисление некоторой математической функции, результатом которого является синаптическое воздействие или его отсутствие.","С помощью класса Population вы можете передать данные популяций на бекенд для вычисления их атрибутов. После вычисления новые атрибуты популяций хранятся на бекенде. Чтобы передать новые атрибуты от бекенда в соответствующие объекты популяций, вы можете вызвать синхронизацию. Если вы вручную изменили атрибуты популяции до вызова синхронизации, вы можете заново передать данные популяции на бекенд для вычисления новых атрибутов. Подробнее о методах популяций и бекендов см. API-документацию Kaspersky Neuromorphic Platform.","Каждая популяция нейронов содержит следующие атрибуты:","Базовые данные популяции.","Тип нейронов, которые хранятся в популяции. ","Тип нейрона определяет набор атрибутов и функцию этого нейрона. Функция нейрона реализуется для конкретного типа нейрона на бекенде. При получении синаптического воздействия функция нейрона изменяет значения его атрибутов и возвращает состояние генерации спайка.","Атрибуты нейронов популяции. Состояние нейронов в популяции.","Класс Population","235842.htm");
Page[18]=new Array("Шаблонный класс Projection реализует контейнер, который содержит синапсы одного из типов, поддерживаемых библиотекой характеристик синапсов.","Место соединения нейронов, основная функция которого&nbsp;&#8211; передача сигнала от одного нейрона к другому. Каждый нейрон может быть связан с множеством синапсов.","Объект класса Projection представляет собой набор соединений нейронов пресинаптической и постсинаптической популяций. Конструирование проекции осуществляется с помощью генератора соединений.","С помощью класса Projection вы можете передавать данные проекции на бекенд и получать их от бекенда. ","Kaspersky Neuromorphic Platform обеспечивает обмен данными между популяциями нейронов и проекциями синапсов через сообщения. Проекция синапсов получает сообщения со спайками от пресинаптической популяции и отправляет сообщения с синаптическими воздействиями в постсинаптическую популяцию.","Значение, формируемое синапсом и предназначенное для изменения значений атрибутов нейронов.","Кратковременный потенциал действия, сгенерированный нейроном популяции в результате изменения атрибутов нейрона.","Каждая проекция синапсов содержит следующие атрибуты:","Базовые данные проекции.","Уникальный идентификатор пресинаптической популяции.","Уникальный идентификатор постсинаптической популяции.","Значение типа boolean для обозначения возможности изменения веса синапса. Если изменение веса синапса заблокировано, то атрибут имеет значение true, иначе имеет значение false.","Атрибуты группы синапсов.","Значение типа boolean для обозначения актуальности реестра соединений. Если реестр соединений актуален, то атрибут имеет значение true, иначе имеет значение false.","Атрибуты, используемые синапсами одного типа.","Класс Projection","235844.htm");
Page[19]=new Array("Класс Backend является базовым для реализаций бекендов. Каждый бекенд реализуется в виде отдельной библиотеки, с помощью которой вы можете:","запускать эмуляцию работы импульсной нейронной сети на различных устройствах;","считывать выходные спайки, полученные от вычислителя;","Кратковременный потенциал действия, сгенерированный нейроном популяции в результате изменения атрибутов нейрона.","получать внутренние данные для мониторинга работы нейронной сети;","выгружать внутренние данные мониторинга работы нейронной сети, полученные от вычислителя.","Каждый объект класса Backend содержит следующие атрибуты:","Базовые данные бекенда.","Шину сообщений. С помощью шины сообщений бекенд обеспечивает обмен сообщениями между объектами библиотеки поддержки бекендов, а также обмен сообщениями от модификаторов к нейронам и синапсам.","Место соединения нейронов, основная функция которого&nbsp;&#8211; передача сигнала от одного нейрона к другому. Каждый нейрон может быть связан с множеством синапсов.","Узел нейронной сети, имеющий набор атрибутов. На основе атрибутов нейрона и последовательности входных сигналов происходит вычисление некоторой математической функции, результатом которого является синаптическое воздействие или его отсутствие.","Код, изменяющий поведение нейрона или синапса, например добавляющий синаптическую пластичность.","Точку подключения.","Статус инициализации бекенда.","Статус исполнения нейронной сети на бекенде.","Устройства бекенда.","Номер шага.","Класс Backend","235749.htm");
Page[20]=new Array("Класс Device является базовым для классов устройств, поддерживаемых библиотекой устройств.","Наследники класса переопределяют методы, с помощью которых вы можете получить общую информацию об объектах классов-наследников. Например, вы можете получить информацию о типе или энергопотреблении конкретного устройства. Данные об устройствах класс Device получает от бекенда.","Каждый объект класса Device содержит базовые данные для конкретного устройства.","Класс Device","245461.htm");
Page[21]=new Array("Класс MessageBus реализует интерфейс для доступа к шине сообщений, которая обеспечивает прием, распределение и доставку сообщений между подключенными к ней объектами. Объект класса MessageBus предоставляет множество точек подключения, с помощью которых объекты библиотеки поддержки бекендов могут подписаться на сообщения или отправить их. ","Класс MessageBus предоставляет методы, которые возвращают используемые точки подключения. Если вы удалите точку подключения, то передача сообщений с помощью этой точки подключения прекратится.","Использование шины сообщений позволяет реализовать принцип \"издатель-подписчик\". Издатель отправляет сообщения через посредника, в качестве которого выступает шина сообщений. Объект подписки фильтрует сообщения и отправляет их подписчикам. Подобный подход позволяет избежать создания большого количества IPC-каналов, поскольку издатель и подписчики не связаны напрямую.","Класс MessageBus","244932.htm");
Page[22]=new Array("Класс MessageEndpoint реализует интерфейс для доступа к точке подключения, которая обеспечивает подписку на сообщения и их отправку с помощью шины сообщений. ","Объект класса MessageEndpoint хранит в себе набор объектов класса Subscription, которые используются для фильтрации сообщений. Шина сообщений может иметь несколько точек подключения. ","С помощью методов класса MessageEndpoint вы можете настроить обмен сообщениями между объектами бекенда и фреймворка. Для передачи сообщений требуется предварительно добавить объект подписки в объект точки подключения. ","Если вы удалите точку подключения, то связанные объекты подписки также будут удалены. Передача сообщений с помощью этой точки подключения прекратится.","Класс MessageEndpoint","247159.htm");
Page[23]=new Array("Класс Subscription реализует функциональность подписки на сообщения. Объект подписки используется для хранения сообщений определенного типа до их запроса получателем. В объекте подписки определены получатели хранящихся сообщений. ","Подписки хранятся в точках подключения к шине сообщений. Если объект точки подключения, который хранит объект подписки, будет удален, то объект подписки будет также удален. Передача сообщений, определенных этой подпиской, прекратится.","Каждый объект подписки содержит следующие атрибуты:","Вектор сообщений для передачи получателю. ","Идентификатор получателя сообщений.","Идентификаторы отправителей сообщений.","Класс Subscription","250697.htm");
Page[24]=new Array("Пространство имен Messaging реализует интерфейсы для доступа к сообщениям. Сообщение содержит данные, которыми обмениваются объекты бекенда, а также данные, отправляемые объектами, которые идентифицируются по UID. Обмен сообщениями обеспечивается с помощью шины сообщений MessageBus.","Каждое сообщение содержит следующие атрибуты:","MessageHeader&nbsp;&#8211; заголовок сообщения, содержащий следующую информацию:","sender_uid_&nbsp;&#8211; идентификатор объекта, отправившего сообщение;","send_time_&nbsp;&#8211; время отправки сообщения.","Тело сообщения, состоящее из совокупности передаваемых атрибутов. Состав передаваемых атрибутов зависит от типа сообщения.","С помощью сообщений происходит обновление пользовательских структур и синхронизация изменений на бекенде. Таблица ниже содержит описание поддерживаемых типов сообщений бекенда. ","Тип сообщения","Описание","Структура сообщения","SpikeMessage","Сообщение от популяции нейронов к проекции синапсов, содержащее спайк. На сообщения, содержащие спайки, может быть подписано более одной проекции.","Кратковременный потенциал действия, сгенерированный нейроном популяции в результате изменения атрибутов нейрона.","header_ &#8211; заголовок сообщения, содержащий UID популяции, нейроны которой сгенерировали спайки, и время генерации спайков.","neuron_indexes_ &#8211; индексы нейронов популяции, сгенерировавших спайки.","SynapticImpactMessage","Сообщение от проекции синапсов к популяции нейронов, содержащее синаптическое воздействие.","Значение, формируемое синапсом и предназначенное для изменения значений атрибутов нейронов.","header_ &#8211; заголовок сообщения, содержащий UID проекции и время отправки сообщения.","postsynaptic_population_uid_ &#8211; UID постсинаптической популяции.","presynaptic_population_uid_ &#8211; UID пресинаптической популяции.","is_forcing_&nbsp;&#8211; значение типа boolean для обозначения использования пластичности в проекции синапсов. Если в проекции синапсов используется синаптическая пластичность, то атрибут имеет значение true, иначе имеет значение false. ","impacts_ &#8211; вектор, содержащий следующие атрибуты:","connection_index_ &#8211; индекс соединения.","impact_value_ &#8211; значение синаптического воздействия.","synapse_type_&nbsp;&#8211; тип синапса.","presynaptic_neuron_index_ &#8211; индекс пресинаптического нейрона. ","postsynaptic_neuron_index_ &#8211; индекс постсинаптического нейрона, которому будет передано значение синаптического воздействия для изменения значений его атрибутов.","Пространство имен Messaging","243634.htm");
Page[25]=new Array("Пространство имен synapse_access реализует соединения и интерфейс для доступа к их реестру.","Структура Connection","Структура Connection определяет соединение синапса с пресинаптическим и постсинаптическим нейроном и включает в себя в следующие атрибуты:","from_&nbsp;&#8211; индекс пресинаптического нейрона.","to_&nbsp;&#8211; индекс постсинаптического нейрона.","index_&nbsp;&#8211; индекс синапса.","Класс Index","Класс Index реализует реестр соединений. С помощью методов класса Index проекция может найти синапсы, связанные с пресинаптическими или постсинаптическими нейронами с заданными индексами. В свою очередь вы можете получить информацию о соединениях с помощью методов проекции.","Запись о соединении синапса с пресинаптическим и постсинаптическим нейронами в реестр выполняется проекцией. Обновление реестра зависит от поведения функции проекции и может выполняться автоматически после любого изменения проекции (например, удаления синапса из проекции) или в момент, когда проекция используется для вычисления.","Пространство имен synapse_access","262482.htm");
Page[26]=new Array("Библиотека устройств реализует и предоставляет набор интерфейсов, которые вы можете использовать для взаимодействия с физическими устройствами. ","При запуске эмуляции импульсной нейронной сети библиотека устройств используется для получения информации об устройствах (например, параметров CPU), на которых работают реализованные бекенды. Информацию об устройстве (например, о потребляемой им мощности) вы можете получить с помощью методов объекта, который обеспечивает интерфейс к устройству.","Выбор устройства для эмуляции нейронной сети осуществляется с помощью методов бекенда. ","В Kaspersky Neuromorphic Platform на текущий момент реализован интерфейс для центрального процессора. ","Библиотека устройств","247186.htm");
Page[27]=new Array("Библиотека характеристик нейронов представляет собой набор атрибутов нейронов. Вы можете использовать атрибуты нейронов, представленных в библиотеке, например, при построении популяций.","Узел нейронной сети, имеющий набор атрибутов. На основе атрибутов нейрона и последовательности входных сигналов происходит вычисление некоторой математической функции, результатом которого является синаптическое воздействие или его отсутствие.","На текущий момент библиотека поддерживает наборы свойств для следующих нейронов:","Нейрон по модели BLIFATNeuron (далее также \"BLIFAT-нейроны\").","Нейрон по модели BLIFATNeuron представляет собой нейрон-интегратор с утечкой и адаптивным порогом, генерирующих серию спайков (англ. Bursting Leaky Integrate-and-Fire with Adaptive Threshold Neuron). BLIFAT-нейрон характеризуется потенциалом мембраны и пороговым значением потенциала мембраны. ","Кратковременный потенциал действия, сгенерированный нейроном популяции в результате изменения атрибутов нейрона.","Значение потенциала мембраны изменяется в соответствии со значением синаптического воздействия, поступающего от связанного синапса. Если потенциал мембраны нейрона достигает порогового значения, нейрон начинает генерировать серию спайков с определенным интервалом. При этом после генерации спайка потенциал мембраны стремится к базовому значению, а пороговое значение изменяется в соответствии с инкрементом. ","Место соединения нейронов, основная функция которого&nbsp;&#8211; передача сигнала от одного нейрона к другому. Каждый нейрон может быть связан с множеством синапсов.","Значение, формируемое синапсом и предназначенное для изменения значений атрибутов нейронов.","Если к нейрону не поступают сообщения от связанного синапса в течение определенного времени, потенциал мембраны и его пороговое значение экспоненциально стремятся к базовым значениям. ","Вы можете указать базовое значения потенциала мембраны, базовое пороговое значение, значение инкремента, интервал времени, при достижении которого значения потенциала и порогового значения потенциала мембраны начинают стремиться к базовым, продолжительность серии спайков и интервал генерации спайков в атрибутах экземпляра нейрона. ","Нейрон по модели SynapticResourceSTDPBLIFATNeuron.","Нейрон по модели SynapticResourceSTDPBLIFATNeuron представляет собой BLIFAT-нейрон, который характеризуется свободным синаптическим ресурсом, пороговым значением свободного синаптического ресурса и стабильностью. ","Если значение свободного синаптического ресурса по модулю превышает пороговое значение, то этот свободный синаптический ресурс распределяется поровну между всеми пластичными пресинаптическими связями нейрона. После этого значение свободного синаптического ресурса нейрона становится равным 0. Если значение свободного синаптического ресурса по модулю меньше порогового значения, то нейрон присваивает себе синаптический ресурс от связанных синапсов до достижения порогового значения.","Значение стабильности нейрона изменяется в соответствии с синапатической пластичностью, используемой синапсом, и влияет на ее значение пластичности. ","Если требуется, вы можете реализовать и использовать собственные модели нейронов.","Библиотека характеристик нейронов","235859.htm");
Page[28]=new Array("Библиотека характеристик синапсов представляет собой набор атрибутов синапсов. Вы можете использовать синапсы, представленные в библиотеке, например, при построении проекций.","Место соединения нейронов, основная функция которого&nbsp;&#8211; передача сигнала от одного нейрона к другому. Каждый нейрон может быть связан с множеством синапсов.","На текущий момент библиотека поддерживает наборы свойств для следующих синапсов:","Синапс по модели DeltaSynapse&nbsp;(далее также \"дельта-синапс\"). ","После получения спайка от пресинаптической популяции синапс генерирует сообщение, содержащее значение синаптического воздействия. Проекция синапсов отправляет синаптическое воздействие популяции для изменения мембранного потенциала постсинаптического нейрона. При этом отправка сообщения проекцией происходит без временной задержки. ","Контейнер, содержащий набор синапсов одного типа, соединяющих нейроны пресинаптической и постсинаптической популяций. ","Значение, формируемое синапсом и предназначенное для изменения значений атрибутов нейронов.","Контейнер, содержащий набор нейронов и их функцию в соответствии с типом нейронов.","Кратковременный потенциал действия, сгенерированный нейроном популяции в результате изменения атрибутов нейрона.","Отправляемое синаптическое воздействие определяется значением веса дельта-синапса. Вы можете указать вес синапса в атрибутах его экземпляра.","Значение сигнала, который синапс передает нейронам связанной популяции.","Синапс по модели AdditiveSTDPDeltaSynapse.","Синапс по модели AdditiveSTDPDeltaSynapse представляет собой дельта-синапс с синаптической пластичностью, зависимой от времени получения спайка. Если спайк от пресинаптической популяции поступает в проекцию перед генерацией пресинаптического спайка постсинаптическим нейроном, значение веса синапса увеличивается. Если пресинаптический спайк поступает на синапс после генерации спайка постсинаптическим нейроном, значение веса синапса уменьшается. ","Вес синапса определяет значение синаптического воздействия, отправляемого в постсинаптическую популяцию.","Изменение веса синапса рассчитывается по формуле, представленной на рисунке ниже, где:","j соответствует пресинаптическому нейрону.","i соответствует постсинаптическому нейрону.","W соответствует функции синаптической пластичности.","n соответствует порядковому номеру пресинаптического нейрона.","f соответствует порядковому номеру постсинаптического нейрона.","@ соответствует времени получения синапсом спайков от пресинаптического нейрона.","@ соответствует времени генерации спайка постсинаптическим нейроном.","@ соответствует значению изменения веса синапса после получения спайка от пресинаптического нейрона.","Формула для расчета изменения веса синапса по модели AdditiveSTDPDeltaSynapse","Синапс по модели SynapticResourceSTDPDeltaSynapse.","Синапс по модели SynapticResourceSTDPDeltaSynapse представляет собой дельта-синапс с синаптической пластичностью, зависимой от синаптического ресурса. В свою очередь значение синаптического ресурса зависит от используемой синаптической пластичности:","Хеббовская пластичность. ","Значение синаптического ресурса зависит от периода плотной последовательности спайков (далее также \"ППС\"). ППС&nbsp;&#8211; это последовательность генерируемых нейроном нефорсированных спайков, в которой все соседние спайки разделены временем не больше, чем ISImax, где ISImax&nbsp;&#8211; это максимальное допустимое расстояние между спайками в ППС. В свою очередь периодом ППС является интервал времени между моментом начала Хеббовской пластичности перед первым спайком в ППС и одним из следующих моментов времени: достижение максимально допустимого расстояния ISImax после последнего спайка в ППС, форсированное срабатывание или приход спайка на дофаминовый синапс.","При получении хотя бы одного спайка в период ППС синаптический ресурс изменяется на величину текущей Хеббовской пластичности. Значение Хеббовской пластичности зависит от переменной стабильности нейрона и рассчитывается по формуле, представленной на рисунке ниже, где: ","@ соответствует Хеббовской пластичности.","s соответствует переменной стабильности нейрона. В момент первого спайка в период ППС значение стабильности меняется на некоторую константу. Если стабильность уже отрицательная, она не меняется в меньшую сторону.","Формула для расчета Хеббовской пластичности","Безусловная пластичность. ","При ненулевой безусловной пластичности значение синаптического ресурса уменьшается на значение пластичности при получении спайка. Значение безусловной пластичности зависит от переменной стабильности нейрона и рассчитывается по формуле, представленной на рисунке ниже, где: ","@ соответствует безусловной пластичности. ","s соответствует переменной стабильности нейрона. В момент первого спайка в период ППС значение стабильности меняется на некоторую константу. Если стабильность уже отрицательная, она не меняется в меньшую сторону.","Формула для расчета безусловной пластичности","Дофаминовая пластичность. ","Дофаминовая пластичность применима, когда сумма всех дофаминовых синапсов, получивших спайк в текущий шаг исполнения нейронной сети, не равна нулю. В случае дофаминовой пластичности значение синаптического ресурса изменяется по формуле, представленной на рисунке ниже, где:","D соответствует сумме дофаминовых синапсов.  ","s соответствует переменной стабильности нейрона.","Формула для расчета синаптического ресурса при дофаминовой пластичности","В свою очередь сумма дофаминовых синапсов влияет на переменную стабильности нейрона. Если в момент форсированного срабатывания пришло дофаминовое наказание (сумма дофаминовых синапсов меньше нуля), то значение стабильности нейрона уменьшается на значение rD, где r соответствует некоторой константе, а D&nbsp;&#8211; дофаминовому наказанию.","Если в момент форсированного срабатывания пришло дофаминовое вознаграждение (сумма дофаминовых синапсов выше нуля), то значение стабильности нейрона увеличивается на значение rD, где r соответствует некоторой константе, а D&nbsp;&#8211; дофаминовому вознаграждению. Иначе значение стабильности изменяется на значение, представленное на рисунке ниже, где:","D соответствует сумме дофаминовых синапсов.","ISImax соответствует максимально допустимому расстоянию между спайками в ППС.","@ соответствует времени с первого спайка в последнем ППС.","Формула для расчета изменения стабильности нейрона при дофаминовом вознаграждении","Вес синапса определяет значение синаптического воздействия, отправляемого в постсинаптическую популяцию и связан со значением синаптического ресурса формулой, представленной на рисунке ниже, где:","w&nbsp;соответствует весу синапса.","W&nbsp;соответствует синаптическому ресурсу.","@&nbsp;соответствует некоторому случайно заданному минимальному значению веса.","@ соответствует некоторому случайно заданному максимальному значению веса.","Формула для расчета изменения веса в зависимости от синаптического ресурса","Если требуется, вы можете реализовать и использовать собственные модели синапсов.","Библиотека характеристик синапсов","235860.htm");
Page[29]=new Array("Библиотека шаблонов представляет собой набор шаблонов общего назначения. Шаблоны библиотеки используются, например, внутри методов бекенда, обеспечивающих интроспекцию. ","Библиотека шаблонов","245041.htm");
Page[30]=new Array("Фреймворк Kaspersky Neuromorphic Platform, реализованный на языке программирования C++, представляет собой независимые блоки, которые обеспечивают управление бекендами, преобразуют представление нейронной сети в разные форматы и выбирают оптимальную нейронную сеть для запуска. Во фреймворке для C++ реализованы функции, не относящиеся непосредственно к эмуляции импульсных нейронных сетей. ","С помощью фреймворка для C++ Kaspersky Neuromorphic Platform вы можете выполнять следующие задачи:","Загружать структуру нейронной сети из внешних форматов.","Динамически загружать различные бекенды.","Выполнять эмуляции импульсных нейронных сетей на различных бекендах.","Вводить и выводить данные нейронных сетей.","Контролировать выполнение эмуляции.","Реализовывать специальные алгоритмы.","Таблица ниже содержит описание объектов фреймворка для C++.","Объекты фреймворка для C++","Объект","Описание","BackendLoader","Класс, реализующий загрузчик динамических библиотек.","Coordinates","Набор классов, реализующих пространственные координаты нейрона.","input","Пространство имен, в котором реализованы интерфейсы для доступа к каналу ввода и преобразователю данных, поступающих из внешней среды.","MessageObserver","Класс, реализующий наблюдателя.","Model","Класс, реализующий модель.","ModelExecutor","Класс, реализующий исполнитель модели.","Network","Класс, реализующий объект нейронной сети.","output","Пространство имен, в котором реализованы интерфейсы для доступа к каналам вывода.","На рисунке ниже представлена схема взаимодействия объектов фреймворка для C++.","В Kaspersky Neuromorphic Platform объект класса Network может включать в себя набор координат (произвольных или задаваемых пользователем) из классов Coordinates. Объект класса Network, а также контейнеры каналов ввода и вывода, реализованных в пространствах имен input и output соответственно, хранятся в объекте класса Model. Каналы ввода и вывода используются для преобразования данных, поступающих в проекции нейронной сети, а также для преобразования сообщений популяций нейронной сети в один из предусмотренных форматов данных.","Вы можете исполнить объект класса Model с помощью метода run() объекта класса ModelExecutor. Объект класса ModelExecutor также содержит объект класса MessageObserver, который обрабатывает полученные сообщения в соответствии с заданной функцией, и экземпляр класса BackendLoader, с помощью методов которого вы можете загружать динамические библиотеки. ","Схема взаимодействия объектов фреймворка для C++","Компоненты фреймворка для C++","235050.htm");
Page[31]=new Array("В пространстве имен input реализованы интерфейсы для доступа к каналу ввода и преобразователю данных, поступающих из внешней среды. Данные могут поступать в Kaspersky Neuromorphic Platform как в синхронном&nbsp;(по запросу пользователя), так и в асинхронном режиме&nbsp;(по вызову обработчика). ","Класс SequenceConverter","Класс SequenceConverter реализует преобразователь данных, который принимает данные из потока istream и делит поступившие данные на отрезки определенной длины. С помощью методов пространства имен input преобразователь данных приводит значения во входных отрезках к типу значений boolean. Если входное значение содержит спайк, метод возвращает true. Объект класса SequenceConverter возвращает список индексов входных значений, которые содержат спайки, в виде сообщения SpikeMessage. ","Кратковременный потенциал действия, сгенерированный нейроном популяции в результате изменения атрибутов нейрона.","Каждый объект класса SequenceConverter содержит следующие атрибуты:","Поток данных istream, от которого поступают данные.","Функцию приведения входных значений к типу значений boolean.","Длину отрезка входных данных. ","Класс IndexConverter","Класс IndexConverter реализует преобразователь данных, который принимает данные из потока istream и преобразует последовательность индексов поступивших значений, содержащих спайки, в сообщение SpikeMessage. ","Каждый объект класса IndexConverter содержит следующие атрибуты:","Поток данных istream, от которого поступают данные.","Символ, с помощью которого индексы значений разделены в сообщении SpikeMessage.","Класс InputChannel","Класс InputChannel реализует канал ввода, который принимает сообщения SpikeMessage от преобразователя данных. С помощью методов класса InputChannel преобразованные данные могут быть переданы подписчикам&nbsp;через точки подключения при наличии подписки.","Каждый объект класса InputChannel содержит следующие атрибуты:","Базовые данные канала ввода.","Точку подключения, с помощью которой преобразованные данные отправляются подписчикам.","Преобразователь данных в сообщения SpikeMessage.","Пространство имен input","244944.htm");
Page[32]=new Array("В пространство имен output реализованы интерфейс для доступа к каналам вывода и преобразователи сообщений SpikeMessage. Kaspersky Neuromorphic Platform может передавать данные как в синхронном режиме&nbsp;(по запросу пользователя), так и в асинхронном режиме&nbsp;(по вызову обработчика).","Класс OutputChannel","Класс OutputChannel реализует канал вывода, который принимает набор сообщений, содержащих спайки, от связанной популяции за несколько тактов исполнения нейронной сети. Сообщения SpikeMessage могут быть преобразованы в один из следующих форматов данных, определяемых преобразователями сообщений:","Вектор значений типа boolean.","Вектор значений, в котором каждое значение соответствует количеству генерации спайков нейроном.","Вектор нейронов, сгенерировавших спайки, из последовательности сообщений SpikeMessage.","Если эти форматы данных вам не подходят, вы можете реализовать собственную функцию, которая будет преобразовывать полученные сообщения в нужный вам формат данных.","Класс ConvertToSet","Класс ConvertToSet реализует преобразователь данных, который получает сообщения SpikeMessage из объекта класса OutputChannel и преобразует их в вектор нейронов, сгенерировавших спайки. ","Каждый объект класса ConvertToSet содержит размер вектора нейронов.","Функтор converter_bitwise","Функтор converter_bitwise реализует преобразователь данных, который получает сообщения SpikeMessage и преобразует их в вектор значений типа boolean, в котором значение true соответствуют наличию спайка.","Функтор converter_count","Функтор converter_count реализует преобразователь данных, который получает сообщения SpikeMessage и преобразует их в вектор значений. Каждое значение вектор соответствует количеству генерации спайков нейроном.","Пространство имен output","243539.htm");
Page[33]=new Array("Набор классов Coordinates реализует координаты точки в n-мерном пространстве. Координаты хранятся в объекте тега. ","Координаты могут быть использованы для отображения нейронной сети. С помощью координат вы также можете задать атрибуты нейронной сети. Например, на основе расстояния между координатами нейронов вы можете выявить задержку сигнала, который передается между этими нейронами.","Координаты могут изменяться произвольно. Во фреймворке для C++ реализованы декартова и полярная системы координат.","Набор классов Coordinates","235753.htm");
Page[34]=new Array("Класс MessageObserver реализует наблюдателя, который подписывается на некоторый набор сущностей и определенный тип сообщений. В конце каждого такта исполнения нейронной сети наблюдатель получает сообщения, пришедшие от сущностей по подписке, и обрабатывает их в соответствии с заданной функцией.","Каждый объект класса MessageObserver содержит следующие атрибуты:","Точку подключения, которая обеспечивает подписку на сообщения сущностей;","Функцию для обработки полученных сообщений;","Идентификатор наблюдателя.","Класс MessageObserver","260375.htm");
Page[35]=new Array("Класс Network реализует объект нейронной сети. Объект нейронной сети хранит базовые данные, популяции, проекции и стратегии генерации нейронов и синапсов, а также обеспечивает общий интерфейс для доступа к нейронам и синапсам нейронной сети. В нейронной сети все популяции связаны проекциями.","Класс содержит методы, с помощью которых вы можете добавлять и удалять популяции и проекции нейронной сети.","Класс Network","235801.htm");
Page[36]=new Array("Класс Model реализует модель, связывающую и хранящую нейронную сеть и каналы ввода и вывода. Для исполнения модели на разных бекендах вы можете загрузить объект модели в объект класса ModelExecutor и вызвать метод исполнения модели run().","С помощью методов класса Model вы можете управлять объектом нейронной сети. ","Каждый объект класса Model содержит следующие атрибуты:","Базовые данные модели.","Нейронную сеть.","Идентификаторы каналов ввода и вывода.","Класс Model","235849.htm");
Page[37]=new Array("Класс ModelExecutor реализует исполнитель моделей. С помощью класса ModelExecutor вы можете исполнить загруженную модель на разных бекендах.","Каждый объект класса ModelExecutor содержит следующие атрибуты:","Базовые данные исполнителя модели.","Загрузчик динамических библиотек.","Экземпляр бекенда, на котором требуется исполнить модель.","Модель для исполнения.","Карту канала ввода, содержащую идентификатор, хешированный идентификатор и преобразователь входных данных.","Каналы ввода и вывода.","Класс ModelExecutor","251296.htm");
Page[38]=new Array("Класс BackendLoader реализует загрузчик динамических библиотек, возвращающий указатель на экземпляр загруженного бекенда. ","С помощью методов класса BackendLoader вы можете загружать динамические библиотеки и проверять, является ли загруженная библиотека библиотекой бекенда.","Класс BackendLoader реализован с помощью библиотеки Boost:DLL.","Класс BackendLoader","243548.htm");
Page[39]=new Array("Этот раздел содержит описание компонентов для использования Kaspersky Neuromorphic Platform на языке программирования Python.","Компоненты платформы для использования на Python","272176.htm");
Page[40]=new Array("Для реализации бекендов вы можете использовать следующие библиотеки платформы на языке программирования Python:","Библиотека поддержки бекендов. Реализует основные объекты и шаблоны вычислителя.","Библиотека характеристик нейронов. Содержит наборы свойств и типизацию популяций моделей нейронов.","Узел нейронной сети, имеющий набор атрибутов. На основе атрибутов нейрона и последовательности входных сигналов происходит вычисление некоторой математической функции, результатом которого является синаптическое воздействие или его отсутствие.","Библиотека характеристик синапсов. Содержит набор классов, определяющих поведение синапса.","Место соединения нейронов, основная функция которого&nbsp;&#8211; передача сигнала от одного нейрона к другому. Каждый нейрон может быть связан с множеством синапсов.","На рисунке ниже представлена схема взаимодействия библиотек платформы.","В Kaspersky Neuromorphic Platform вычислитель хранит в себе экземпляры объектов библиотеки поддержки бекендов (например, проекции и популяции), необходимые для выполнения прикладных задач на вычислителе. Шаблоны популяций и проекций нейронной сети должны быть специализированы типами нейронов и синапсов, описанными в библиотеках характеристик нейронов и синапсов. ","Контейнер, содержащий набор нейронов и их функцию в соответствии с типом нейронов.","Контейнер, содержащий набор синапсов одного типа, соединяющих нейроны пресинаптической и постсинаптической популяций. ","Компоненты бекенда платформы для Python","272177.htm");
Page[41]=new Array("В библиотеке поддержки бекендов реализуются интерфейсы к объектам, необходимым для работы бекендов и выполнения прикладных задач. Библиотека содержит пространство имен core.","Таблица ниже содержит описание объектов библиотеки.","Объект","Описание","Backend","Базовый класс для конкретных реализаций бекендов.","BaseData","Структура, определяющая набор базовых данных.","continuously_uid_generator","Класс, реализующий генератор уникальных идентификаторов.","MessageBus","Класс, реализующий интерфейс к шине сообщений.","MessageEndpoint","Класс, реализующий интерфейс к точке подключения.","Messaging","Пространство имен, определяющее интерфейсы к сообщениям.","Population","Набор классов, реализующих контейнеры нейронов одной модели.","Узел нейронной сети, имеющий набор атрибутов. На основе атрибутов нейрона и последовательности входных сигналов происходит вычисление некоторой математической функции, результатом которого является синаптическое воздействие или его отсутствие.","Projection","Набор классов, реализующих контейнеры синапсов одного типа.","Место соединения нейронов, основная функция которого&nbsp;&#8211; передача сигнала от одного нейрона к другому. Каждый нейрон может быть связан с множеством синапсов.","Subscription","Набор классов, реализующих подписку на сообщения.","TagMap","Класс, реализующий интерфейс к словарю тегов.","UID","Класс, определяющий уникальные идентификаторы.","uid_hash","Класс, реализующий хеширования UID.","UUID","Класс, реализующий интерфейс к уникальному идентификатору библиотеки Boost:Uuid.","На рисунке ниже представлена схема взаимодействия объектов библиотеки.","В Kaspersky Neuromorphic Platform структура BaseData содержит базовую информацию об объекте и включает в себя UID и TagMap. Структура BaseData используется в классах Projection, Population и Backend, соответственно каждый из них обладает идентификатором UID и набором тегов и их значениями TagMap.","Классы Population и Projection принимают и отправляют сообщения из пространства имен Messaging, которые содержат в своем заголовке UID отправителя. Обмен сообщений осуществляется с помощью методов класса Backend, в котором хранятся классы MessageBus и MessageEndpoint. Шина сообщений MessageBus создает точки подключения MessageEndpoint и взаимодействует с ними в процессе обмена сообщениями. Каждая точка подключения MessageEndpoint содержит контейнер подписок класса Subscription.","Схема взаимодействия объектов библиотеки поддержки бекендов","Библиотека поддержки бекендов","272188.htm");
Page[42]=new Array("Объект класса UID является частью структуры BaseData и представляет собой уникальный идентификатор объекта. ","Если требуется, вы можете сгенерировать случайный UID или создать уникальный идентификатор UUID. Класс UID также предоставляет логические операторы и операторы сравнения и присваивания.","Класс UID","272192.htm");
Page[43]=new Array("Класс uid_hash реализует функцию хеширования идентификаторов объектов.","Класс uid_hash","272227.htm");
Page[44]=new Array("Класс UUID реализует интерфейс к уникальному идентификатору библиотеки Boost:Uuid. Каждый уникальный идентификатор имеет следующие свойства:","Размер уникального идентификатора в байтах.","Функцию, возвращающую значение true, если уникальный идентификатор состоит исключительно из нулей.","Один из следующих вариантов идентификатора: ","NCS&nbsp;&#8211; вариант, зарезервированный для обратной совместимости с форматом UUID Apollo Network Computing System 1.5.","RFC_4122&nbsp;&#8211; вариант, описанный в RFC 4122.","MICROSOFT&nbsp;&#8211; вариант, зарезервированный для обратной совместимости с ранними GUID Microsoft Windows.","FUTURE&nbsp;&#8211; вариант, зарезервированный для использования в будущем.","Класс UUID","272229.htm");
Page[45]=new Array("Класс continuously_uid_generator реализует генератор уникальных идентификаторов&nbsp;(англ. UID). Идентификатор отображается в виде строки, разбитой на группы с помощью дефисов, и представляет собой 128-битное число. Идентификаторы генерируются последовательно с шагом, равным 1.","Вы можете использовать класс continuously_uid_generator для отладки платформы и исполнения нейронной сети.","Класс continuously_uid_generator","272193.htm");
Page[46]=new Array("Объект класса TagMap является составляющей частью структуры BaseData и представляет собой ассоциативный массив, включающий ноль или более записей типа \"ключ-значение тега\", где ключом является имя тега. Имена тегов имеют строковый тип и являются уникальными. ","Класс TagMap","272194.htm");
Page[47]=new Array("Структура BaseData определяет набор базовых данных, общих для нескольких объектов библиотеки поддержки бекендов и объектов фреймворка. Например, базовые данные BaseData используются в объектах классов Backend, Population и Projection.","Базовые данные BaseData включают в себя следующее:","uid_&nbsp;&#8211; уникальный идентификатор объекта.","tags_&nbsp;&#8211; ассоциативный массив TagMap, состоящий из тегов, используемых объектом, и их значений.","Структура BaseData","272195.htm");
Page[48]=new Array("Набор классов Population реализует контейнеры, каждый из которых содержит нейроны, принадлежащие одной модели. При использовании набора классов Population вы можете указать один из типов нейронов, поддерживаемых библиотекой характеристик нейронов. Каждая популяция нейронов имеет уникальный идентификатор.","Узел нейронной сети, имеющий набор атрибутов. На основе атрибутов нейрона и последовательности входных сигналов происходит вычисление некоторой математической функции, результатом которого является синаптическое воздействие или его отсутствие.","С помощью набора классов Population вы можете управлять нейронами в составе популяции. Вы также можете загрузить популяции на бекенд для вычисления нейронной сети. Полученные свойства популяций хранятся на бекенде. Чтобы передать новые свойства от бекенда в соответствующие объекты популяций, вы можете вызвать синхронизацию. Если вы вручную изменили свойства популяции до вызова синхронизации, вы можете заново загрузить популяцию на бекенд для вычисления нейронной сети. Подробнее о методах популяций и бекендов см. API-документацию Kaspersky Neuromorphic Platform.","Набор классов Population","272196.htm");
Page[49]=new Array("Набор классов Projection реализует контейнеры, каждый из которых содержит синапсы одного из типов, поддерживаемых библиотекой характеристик синапсов. Каждая проекция синапсов имеет уникальный идентификатор.","Место соединения нейронов, основная функция которого&nbsp;&#8211; передача сигнала от одного нейрона к другому. Каждый нейрон может быть связан с множеством синапсов.","В классах проекций хранятся синапсы, представляющие собой соединения между нейронами пресинаптической и постсинаптической популяций. Идентификаторы этих популяций хранятся в проекции. ","С помощью набора классов Projection вы можете управлять синапсами в составе проекции, а также передавать соединения между нейронами на бекенд и получать их от бекенда обратно. ","Набор классов Projection","272197.htm");
Page[50]=new Array("Класс Backend является базовым для реализаций бекендов. Каждый бекенд реализуется в виде отдельной библиотеки, с помощью которой вы можете:","запускать эмуляцию работы импульсной нейронной сети на различных устройствах;","получать сообщения, содержащие спайки и/или синаптические воздействия, для заданной точки подключения;","получать внутренние данные для мониторинга работы нейронной сети;","выгружать внутренние данные мониторинга работы нейронной сети, полученные от вычислителя.","Каждый объект класса Backend имеет следующие свойства:","Уникальный идентификатор бекенда.","Шину сообщений. С помощью шины сообщений бекенд обеспечивает обмен сообщениями между объектами библиотеки поддержки бекендов, а также обмен сообщениями от модификаторов к нейронам и синапсам.","Место соединения нейронов, основная функция которого&nbsp;&#8211; передача сигнала от одного нейрона к другому. Каждый нейрон может быть связан с множеством синапсов.","Узел нейронной сети, имеющий набор атрибутов. На основе атрибутов нейрона и последовательности входных сигналов происходит вычисление некоторой математической функции, результатом которого является синаптическое воздействие или его отсутствие.","Код, изменяющий поведение нейрона или синапса, например добавляющий синаптическую пластичность.","Точку подключения.","Статус исполнения нейронной сети на бекенде.","Класс Backend","272198.htm");
Page[51]=new Array("Класс MessageBus реализует интерфейс для доступа к шине сообщений, которая обеспечивает прием, распределение и доставку сообщений между подключенными к ней объектами. Объект класса MessageBus предоставляет множество точек подключения, с помощью которых объекты библиотеки поддержки бекендов могут подписаться на сообщения или отправить их. ","Класс MessageBus предоставляет методы, с помощью которых вы можете создать точки подключения, а также передать сообщения через шину сообщений. Если вы удалите точку подключения, то передача сообщений с помощью этой точки подключения прекратится.","Использование шины сообщений позволяет реализовать принцип \"издатель-подписчик\". Издатель отправляет сообщения через посредника, в качестве которого выступает шина сообщений. Объект подписки фильтрует сообщения и отправляет их подписчикам. Подобный подход позволяет избежать создания большого количества IPC-каналов, поскольку издатель и подписчики не связаны напрямую.","Класс MessageBus","272199.htm");
Page[52]=new Array("Класс MessageEndpoint реализует интерфейс для доступа к точке подключения, которая обеспечивает подписку на сообщения и их отправку с помощью шины сообщений. ","Объект класса MessageEndpoint хранит в себе идентификатор получателя сообщений и ключ класса подписки, который используется для фильтрации сообщений. Шина сообщений может иметь несколько точек подключения. ","С помощью методов класса MessageEndpoint вы можете настроить обмен сообщениями между объектами бекенда и фреймворка с заданными идентификаторами. Для передачи сообщений требуется предварительно добавить ключ класса подписки в объект точки подключения. ","Если вы удалите точку подключения, то связанные классы подписки также будут удалены. Передача сообщений с помощью этой точки подключения прекратится.","Класс MessageEndpoint","272200.htm");
Page[53]=new Array("Набор классов Subscription реализует функциональность подписки на сообщения. Класс из группы подписок Subscription используется для хранения сообщений определенного типа до их запроса получателем. С помощью методов набора классов Subscription вы можете определить получателей хранящихся сообщений. ","Ключи классов подписки хранятся в точках подключения к шине сообщений. Если объект точки подключения, который хранит ключ класса, будет удален, то соответствующий класс подписки будет также удален. Передача сообщений, определенных этим классом, прекратится.","Набор классов Subscription","272201.htm");
Page[54]=new Array("Пространство имен Messaging реализует интерфейсы для доступа к сообщениям. Сообщение содержит данные, которыми обмениваются объекты бекенда, а также данные, отправляемые объектами, которые идентифицируются по UID. Обмен сообщениями обеспечивается с помощью шины сообщений MessageBus.","Каждое сообщение содержит следующие свойства:","MessageHeader&nbsp;&#8211; заголовок сообщения, содержащий следующую информацию:","sender_uid&nbsp;&#8211; идентификатор объекта, отправившего сообщение;","send_time&nbsp;&#8211; время отправки сообщения.","Тело сообщения, состоящее из совокупности передаваемых атрибутов. Состав передаваемых свойств зависит от типа сообщения.","С помощью сообщений происходит обновление пользовательских структур и синхронизация изменений на бекенде. Таблица ниже содержит описание поддерживаемых типов сообщений бекенда. ","Тип сообщения","Описание","Структура сообщения","SpikeMessage","Сообщение от популяции нейронов к проекции синапсов, содержащее спайк. На сообщения, содержащие спайки, может быть подписано более одной проекции.","Кратковременный потенциал действия, сгенерированный нейроном популяции в результате изменения атрибутов нейрона.","header &#8211; заголовок сообщения, содержащий UID популяции, нейроны которой сгенерировали спайки, и время генерации спайков.","neuron_indexes &#8211; индексы нейронов популяции, сгенерировавших спайки.","SynapticImpactMessage","Сообщение от проекции синапсов к популяции нейронов, содержащее синаптическое воздействие.","Значение, формируемое синапсом и предназначенное для изменения значений атрибутов нейронов.","header &#8211; заголовок сообщения, содержащий UID проекции и время отправки сообщения.","postsynaptic_population_uid_ &#8211; UID постсинаптической популяции.","presynaptic_population_uid_ &#8211; UID пресинаптической популяции.","is_forcing_&nbsp;&#8211; значение типа boolean для обозначения использования пластичности в проекции синапсов. Если в проекции синапсов используется синаптическая пластичность, то атрибут имеет значение true, иначе имеет значение false. ","impacts_ &#8211; вектор, содержащий следующие свойства:","connection_index_ &#8211; индекс соединения.","impact_value_ &#8211; значение синаптического воздействия.","synapse_type_&nbsp;&#8211; тип синапса.","presynaptic_neuron_index_ &#8211; индекс пресинаптического нейрона. ","postsynaptic_neuron_index_ &#8211; индекс постсинаптического нейрона, которому будет передано значение синаптического воздействия для изменения значений его свойств.","Пространство имен Messaging","272202.htm");
Page[55]=new Array("Библиотека характеристик нейронов представляет собой набор атрибутов нейронов. Вы можете использовать атрибуты нейронов, представленных в библиотеке, например, при построении популяций.","Узел нейронной сети, имеющий набор атрибутов. На основе атрибутов нейрона и последовательности входных сигналов происходит вычисление некоторой математической функции, результатом которого является синаптическое воздействие или его отсутствие.","На текущий момент библиотека поддерживает наборы свойств для нейрона по модели BLIFATNeuron (далее также \"BLIFAT-нейроны\").","Нейрон по модели BLIFATNeuron представляет собой нейрон-интегратор с утечкой и адаптивным порогом, генерирующих серию спайков (англ. Bursting Leaky Integrate-and-Fire with Adaptive Threshold Neuron). BLIFAT-нейрон характеризуется потенциалом мембраны и пороговым значением потенциала мембраны. ","Кратковременный потенциал действия, сгенерированный нейроном популяции в результате изменения атрибутов нейрона.","Значение потенциала мембраны изменяется в соответствии со значением синаптического воздействия, поступающего от связанного синапса. Если потенциал мембраны нейрона достигает порогового значения, нейрон начинает генерировать серию спайков с определенным интервалом. При этом после генерации спайка потенциал мембраны стремится к базовому значению, а пороговое значение изменяется в соответствии с инкрементом. ","Место соединения нейронов, основная функция которого&nbsp;&#8211; передача сигнала от одного нейрона к другому. Каждый нейрон может быть связан с множеством синапсов.","Значение, формируемое синапсом и предназначенное для изменения значений атрибутов нейронов.","Если к нейрону не поступают сообщения от связанного синапса в течение определенного времени, потенциал мембраны и его пороговое значение экспоненциально стремятся к базовым значениям. ","Вы можете указать базовое значения потенциала мембраны, базовое пороговое значение, значение инкремента, интервал времени, при достижении которого значения потенциала и порогового значения потенциала мембраны начинают стремиться к базовым, продолжительность серии спайков и интервал генерации спайков в атрибутах экземпляра нейрона. ","Если требуется, вы можете реализовать и использовать собственные модели нейронов.","Библиотека характеристик нейронов","272189.htm");
Page[56]=new Array("Библиотека характеристик синапсов представляет собой набор атрибутов синапсов. Вы можете использовать синапсы, представленные в библиотеке, например, при построении проекций.","Место соединения нейронов, основная функция которого&nbsp;&#8211; передача сигнала от одного нейрона к другому. Каждый нейрон может быть связан с множеством синапсов.","На текущий момент библиотека поддерживает наборы свойств для синапса по модели DeltaSynapse&nbsp;(далее также \"дельта-синапс\")."," ","После получения спайка от пресинаптической популяции синапс генерирует сообщение, содержащее значение синаптического воздействия. Проекция синапсов отправляет синаптическое воздействие популяции для изменения мембранного потенциала постсинаптического нейрона. При этом отправка сообщения проекцией происходит без временной задержки. ","Контейнер, содержащий набор синапсов одного типа, соединяющих нейроны пресинаптической и постсинаптической популяций. ","Значение, формируемое синапсом и предназначенное для изменения значений атрибутов нейронов.","Контейнер, содержащий набор нейронов и их функцию в соответствии с типом нейронов.","Кратковременный потенциал действия, сгенерированный нейроном популяции в результате изменения атрибутов нейрона.","Отправляемое синаптическое воздействие определяется значением веса дельта-синапса. Вы можете указать вес синапса в атрибутах его экземпляра.","Значение сигнала, который синапс передает нейронам связанной популяции.","Если требуется, вы можете реализовать и использовать собственные модели синапсов.","Библиотека характеристик синапсов","272190.htm");
Page[57]=new Array("Фреймворк Kaspersky Neuromorphic Platform, реализованный на языке программирования Python, представляет собой независимые блоки, которые обеспечивают управление бекендами, преобразуют представление нейронной сети в разные форматы и выбирают оптимальную нейронную сеть для запуска. Во фреймворке для Python реализованы функции, не относящиеся непосредственно к эмуляции импульсных нейронных сетей.","С помощью фреймворка для Python Kaspersky Neuromorphic Platform вы можете динамически загружать различные бекенды. ","Таблица ниже содержит описание объектов Python-фреймворка. ","Объекты Python-фреймворка","Объект","Описание","ANN2SNN","Библиотека, реализующая тернарные слои нейронной сети.","BackendLoader","Класс, реализующий загрузчик динамических библиотек.","Компоненты Python-фреймворка","240225.htm");
Page[58]=new Array("Библиотека ANN2SNN предназначена для создания нейронных сетей, которые можно преобразовать в импульсные нейронные сети и разместить на нейроморфном процессоре Алтай-2 для их исполнения. Библиотека ANN2SNN реализована на языке программирования Python.","Для создания нейронных сетей необходимо использовать специальные тернарные слои, в которых учитываются ограничения, характерные для нейроморфного процессора Алтай-2. Тернарные слои, используемые для создания нейронной сети, соответствуют слоям библиотеки Keras, со следующими основными отличиями:","Входными и выходными данными каждого из слоев нейронной сети являются бинарные тензоры соответствующей размерности.","Все веса слоев нейронной сети находятся в диапазоне {-1,0,1}.","Значение смещения в слое нейронной сети находится в диапазоне от -32&nbsp;768 до 32&nbsp;767.","При обучении нейронной сети, созданной с помощью тернарных слоев, стоит учитывать следующие особенности:","В случае наличия небинаризованных данных для обучения и/или валидации необходимо сначала их привести к бинарному виду. Для это можно использовать алгоритмы кодирования и декодирования или создать нейросетевые обучаемые блоки, кодирующие и декодирующие данные.","При обучении моделей бинарной классификации методом градиентного спуска для дифференцируемости функции потерь необходимо указать сигмоидную функцию (англ. sigmoid function) в качестве функции активации выходного слоя. После проведения обучения нужно заменить сигмоидноую активацию выходного слоя на активацию с функцией Хевисайда (англ. Heaviside function). ","При обучении моделей многоклассовой классификации методом градиентного спуска для дифференцируемости функции потерь необходимо указать многомерную логистическую функцию (англ. softmax function) в качестве функции активации выходного слоя. При размещении такой нейронной сети на нейроморфном процессоре Алтай-2 выходной вектор будет автоматически преобразован к бинарному виду, реализующему индикатор максимальной компоненты значения выходного вектора. ","Сериализация и десериализация нейронных сетей проводится стандартными методами, предоставляемыми библиотекой Keras, с указанием параметра custom_objects в виде словаря тернарных слоев и функции активации heaviside. ","Во время размещения нейронной сети на нейроморфном процессоре нейронная сеть проверяется на соответствие ограничениям процессора. В результате размещения выдается информация о количестве ядер, необходимых для запуска нейронной сети на нейроморфном процессоре Алтай-2. Если нейронную сеть невозможно разместить на нейроморфном процессоре, выдается соответствующая ошибка. Вы можете провести примерную оценку количества ядер, необходимого для размещения нейронной сети на нейроморфном процессоре Алтай-2, по формуле, представленной на рисунке ниже, где:","x соответствует необходимому количеству ядер. После расчета количества ядер по формуле округлите результат в большую сторону. ","i соответствует порядковому номеру слоя нейронной сети.","n соответствует количеству слоев нейронной сети.","w соответствует первой размерности тензора слоя нейронной сети.","h соответствует второй размерности тензора слоя нейронной сети.","c соответствует третьей размерности тензора слоя нейронной сети.","Формула для примерной оценки необходимого количества ядер","Для нейронных сетей, созданных на базе библиотеки ANN2SNN, возможны следующие варианты инференса:","Инференс за количество тиков, равное количеству слоев в нейронной сети. При этом варианте инференса за тик исполнения нейронной сети вычисляется только один слой нейронной сети, а обработанные данные передаются в последующий слой. Время инференса прямо пропорционально количеству слоев нейронной сети.","Квант времени, за который выполняется вычисление функции всех нейронов в ядре. За время тика все спайки, сгенерированные ядрами или поступившие на входные линии нейронов, доставляются до целевых нейронов.","Инференс за один тик. При этом варианте инференса каждый слой нейронной сети получает данные от предыдущего слоя и обрабатывает их. Время инференса составляет около 1,5&nbsp;мс.","Библиотека ANN2SNN","273090.htm");
Page[59]=new Array("Развернуть все&nbsp;|&nbsp;Свернуть все","В библиотеке ANN2SNN поддерживаются следующие тернарные слои:","Слой TernaryDense","Класс, реализующий полносвязный слой, аналогом которого является слой Dense библиотеки Keras. При работе с полносвязным слоем рекомендуется указать количество нейронов слоя (параметр units) равным или меньше 512. Если у нейрона слоя более 512 связей, то такой нейрон будет представлен в виде нескольких нейронов на нейроморфном процессоре Алтай-2. ","Слой TernaryConv1D","Класс, реализующий одномерный сверточный слой, аналогом которого является слой Conv1D библиотеки Keras. При работе с этим слоем рекомендуется установить следующие ограничения:","Размер ядра (параметр kernel_size)&nbsp;&#8211; не более 20.","Количество фильтров (параметр filters)&nbsp;&#8211; не более 64.","Слой TernaryConv2D","Класс, реализующий двумерный сверточный слой, аналогом которого является слой Conv2D библиотеки Keras. При работе с этим слоем рекомендуется следовать следующим ограничениям:","Не использовать ядра большого размера (параметр kernel_size). Оптимальными считаются ядра с размерами [2,&nbsp;2], [3,&nbsp;3] и [4,&nbsp;4].","Ограничить размер входных изображений до 128x128 и число каналов до 10.","Ограничить количество фильтров (параметр filters) до 32.","Слой TernaryLocallyConnected1D","Класс, реализующий одномерный локально-связный слой, аналогом которого является слой LocallyConnected1D библиотеки Keras. При работе с этим слоем рекомендуется установить следующие ограничения:","Размер ядра (параметр kernel_size)&nbsp;&#8211; не более 20.","Количество фильтров (параметр filters)&nbsp;&#8211; не более 64.","Слой TernaryLocallyConnected2D","Класс, реализующий двумерный локально-связный слой, аналогом которого является слой LocallyConnected2D библиотеки Keras. При работе с этим слоем рекомендуется следовать следующим ограничениям:","Не использовать ядра большого размера (параметр kernel_size). Оптимальными считаются ядра с размерами [2,&nbsp;2], [3,&nbsp;3] и [4,&nbsp;4].","Ограничить размер входных изображений до 128x128 и число каналов до 10.","Ограничить количество фильтров (параметр filters) до 32.","Слой TernaryDepthwiseConv1D","Класс, реализующий одномерный сверточный слой, аналогом которого является слой DepthwiseConv1D библиотеки Keras. При работе с этим слоем рекомендуется установить следующие ограничения:","Размер ядра (параметр kernel_size)&nbsp;&#8211; не более 20.","Количество фильтров (параметр filters)&nbsp;&#8211; не более 64.","Слой TernaryDepthwiseConv2D","Класс, реализующий двумерный сверточный слой, аналогом которого является слой DepthwiseConv2D библиотеки Keras. При работе с этим слоем рекомендуется следовать следующим ограничениям:","Не использовать ядра большого размера (параметр kernel_size). Оптимальными считаются ядра с размерами [2,&nbsp;2], [3,&nbsp;3] и [4,&nbsp;4].","Ограничить размер входных изображений до 128x128 и число каналов до 10.","Ограничить количество фильтров (параметр filters) до 32.","Слой MaxPool1D","Поддерживается стандартная реализация соответствующего слоя библиотеки Keras. Для уменьшения количества оборудования нейроморфного процессора, необходимого для исполнения нейронной сети, рекомендуется отказаться от использования этого слоя путем увеличения параметра stride в сверточных слоях.","Слой MaxPool2D","Поддерживается стандартная реализация соответствующего слоя библиотеки Keras. Для уменьшения количества оборудования нейроморфного процессора, необходимого для исполнения нейронной сети, рекомендуется отказаться от использования этого слоя путем увеличения параметра stride в сверточных слоях.","Слой BatchNormalization","Поддерживается стандартная реализация нормирующего слоя BatchNormalization библиотеки Keras. Параметр масштабирования scale не поддерживается и должен быть определен как false. ","Слой BatchNormalization должен идти после слоя Activation.","Слой InputLayer","Поддерживается стандартная реализация входного слоя InputLayer библиотеки Keras. Входные данные должны иметь бинарный тип. ","Слой Flatten","Поддерживается стандартная реализация соответствующего слоя библиотеки Keras.","Слой Reshape","Поддерживается стандартная реализация соответствующего слоя библиотеки Keras.","Слой Concatenate","Поддерживается стандартная реализация соответствующего слоя библиотеки Keras.","Для размещения нейронной сети на нейроморфном процессоре Алтай-2 в качестве функции активации каждого слоя необходимо указать функцию heaviside. При обучении нейронной сети методом градиентного спуска используется кусочно-линейная аппроксимация сигмоидной функции (англ. hard sigmoid function). ","Тернарные слои библиотеки ANN2SNN","273444.htm");
Page[60]=new Array("Класс BackendLoader реализует загрузчик динамических библиотек, возвращающий указатель на экземпляр загруженного бекенда. ","С помощью методов класса BackendLoader вы можете загружать динамические библиотеки и проверять, является ли загруженная библиотека библиотекой бекенда.","Класс BackendLoader","272051.htm");
Page[61]=new Array("Этот раздел содержит информацию о сторонних библиотеках, форматах данных и структуры нейронной сети, используемых Kaspersky Neuromorphic Platform.","Сторонние библиотеки и используемые форматы","235594.htm");
Page[62]=new Array("Kaspersky Neuromorphic Platform использует следующие библиотеки:","Boost.&nbsp;Библиотеки используются для реализации классов и структур&nbsp;(например, UID, BackendLoader).","FlatBuffers. Библиотека используется для сериализации в Kaspersky Neuromorphic Platform.","GoogleTest. Библиотека используется для тестов платформы.","Intel PCM. Библиотека используется для получения от CPU метаданных устройства&nbsp;(например, данные об энергопотреблении).","spdlog. Библиотека используется для логирования работы Kaspersky Neuromorphic Platform.","Сторонние библиотеки","235876.htm");
Page[63]=new Array("Для хранения входных и выходных данных в виде временного ряда используется базовый формат Kaspersky Neuromorphic Platform. Файл базового формата содержит следующую информацию: ","Заголовок файла, содержащий следующую информацию:","версия формата файла;","массив идентификаторов UID отправителей данных для каждого шага исполнения нейронной сети&nbsp;(опционально);","массив тегов.","Входные или выходные данные представляют собой временной ряд, состоящий из списка структур со следующей информацией:","шаг исполнения нейронной сети, на котором были сгенерированы спайки;","индексы нейронов, сгенерировавших спайки на этой шаге. ","С помощью методов Kaspersky Neuromorphic Platform вы можете преобразовать файлы базового формата в сообщения SpikeMessage.","Сообщения формата KNP сериализуются в следующие нотации:","HDF5&nbsp;&#8211; основная нотация для хранения данных в бинарном виде;","JSON&nbsp;&#8211; нотация объектов JavaScript, которая может быть использована для отладки работы нейронной сети.","Помимо базового формата входные данные также могут храниться и быть переданы в виде CSV-файлов, в которых содержатся индексы нейронов, сгенерировавших спайки. CSV-файлы с входными данными могут быть переданы в объекты класса IndexConverter для их дальнейшей обработки в каналах ввода.","Форматы данных, обрабатываемые платформой","235877.htm");
Page[64]=new Array("Развернуть все&nbsp;|&nbsp;Свернуть все","В Kaspersky Neuromorphic Platform используется формат SONATA для описания топологии нейронной сети.","В формате SONATA нейронная сеть представляется в виде графа, состоящего из вершин, соответствующих нейронам, и ребер, соединяющих вершины графа и соответствующих синапсам. Вершины графа могут быть объединены в несколько популяций нейронов, которые связывается между собой проекцией синапсов (совокупностью ребер графа). Нейронам и синапсам присваиваются типы соответствующих популяций и проекций. Параметры, назначенные типам нейронов или синапсов, автоматически присваиваются всем нейронам или синапсам этих типов. Кроме того, нейронам, синапсам и их типам могут быть назначены атрибуты, которые определяют различные аспекты нейронной сети (например, положение, тип, параметры модели). ","Популяция нейронов состоит из одной или нескольких групп нейронов с единообразной табличной структурой. Популяции сериализуются в HDF5-файлы и связываются между собой CSV-файлом, описывающим типы нейронов и атрибуты, применимые к этим типам. Группы нейронов представляются в виде групп HDF5-файлов, содержащих наборы данных, длины которых соответствуют количеству нейронов в группах. Данные в HDF5-файлах хранятся в бинарном виде.","Аналогично нейронам, синапсы определяются в проекциях, хранящихся в HDF5-файлах. В этих файлах описываются атрибуты для каждого синапса. Проекция синапсов состоит из одной или нескольких групп синапсов с единообразной табличной структурой. Каждый HDF5-файл связан с CSV-файлом, в котором определены типы синапсов и атрибуты, применимые к синапсам этих типов. ","Примеры структур HDF5-файлов:","Структура файла, описывающего группы нейронов","Ниже приведен пример структуры HDF5-файла, описывающего группы нейронов.","&lt;имя_файла&gt;.h5  ","    nodes                            ","        &lt;название_популяции&gt;","            node_type_id","            node_id                   ","            node_group_id             ","            node_group_index         ","            &lt;группа_нейронов&gt;  ","                &lt;атрибут_группы&gt; ","                ...","                dynamics_params  ","                    &lt;динамический_атрибут&gt; ","                    ... ","В таблице ниже приведены элементы данных в файле, описывающем группы нейронов.","Элементы данных файла, описывающего группы нейронов","Элемент данных","Описание","&lt;имя_файла&gt;.h5&nbsp;","Имя файла, описывающего группы нейронов.","nodes","Обязательный элемент для определения групп.","&lt;название_популяции&gt;","Обязательный элемент, содержащий название популяции. Популяций может быть несколько.","node_type_id","Обязательный элемент, содержащий набор данных с массивом идентификаторов типов нейронов популяции.","node_id","Обязательный элемент, содержащий набор данных с индексами нейронов популяции.","node_group_id","Обязательный элемент, содержащий набор данных с индексами групп, к которым относятся нейроны популяции. ","node_group_index","Обязательный элемент. содержащий набор данных с индексами нейронов в соответствующих группах.","&lt;группа_нейронов&gt;","Обязательный элемент, содержащий идентификатор группы нейронов.","&lt;атрибут_группы&gt;","Один или несколько атрибутов группы. Атрибуты не являются обязательными и зависят от типа нейрона. Каждый из атрибутов представляет собой набор данных.","dynamics_params","Группа необязательных динамических атрибутов. ","&lt;динамический_атрибут&gt;","Один или несколько динамических атрибутов. Каждый из атрибутов представляет собой набор данных со значениями атрибутов для каждого нейрона в группе. ","Структура файла, описывающего группы синапсов","Ниже приведен пример структуры HDF5-файла, описывающего группы синапсов.","&lt;имя_файла&gt;.h5  ","    edges                            ","        &lt;название_проекции&gt;","            source_node_id","            edge_group_id                   ","            edge_group_index             ","            target_node_id","            edge_type_id","            indices","                source_to_target","                    node_id_to_range","                    range_to_edge_id","                target_to_source","                    node_id_to_range","                    range_to_edge_id","            &lt;группа_синапсов&gt;","                delay","                syn_weight","                dynamics_params  ","В таблице ниже приведены элементы данных в файле, описывающем группы синапсов.","Элементы данных файла, описывающего группы синапсов","Элемент данных","Описание","&lt;имя_файла&gt;.h5&nbsp;","Имя файла, описывающего группы синапсов.","edges","Обязательный элемент для определения групп.","&lt;название_проекции&gt;","Обязательный элемент, содержащий название проекции. Проекций может быть несколько.","source_node_id","Обязательный элемент, содержащий набор данных с идентификаторами пресинаптических нейронов для каждого синапса проекции.","edge_group_id","Обязательный элемент, содержащий набор данных с идентификаторами групп, к которым относятся синапсы проекции.","edge_group_index","Обязательный элемент, содержащий набор данных с индексами синапсов в соответствующих группах.","target_node_id","Обязательный элемент, содержащий набор данных с идентификаторами постсинаптических нейронов для каждого синапса проекции.","edge_type_id","Обязательный элемент, содержащий набор данных с идентификаторами типа каждого синапса проекции.","indices","Необязательный элемент для индексации синапсов по пресинаптическим и постсинаптическим нейронам.","source_to_target","Элемент, описывающий синапсы по пресинаптическим нейронам.","target_to_source","Элемент, описывающий синапсы по постсинаптическим нейронам.","node_id_to_range","Элемент, содержащий набор данных с идентификаторами пресинаптических нейронов для каждого синапса проекции.","range_to_edge_id","Элемент. содержащий набор данных с идентификаторами постсинаптических нейронов для каждого синапса проекции.","&lt;группа_синапсов&gt;","Обязательный элемент, содержащий идентификатор группы нейронов.","delay","Необязательный элемент, содержащий набор данных со значениями задержек синапсов группы.","syn_weight","Необязательный элемент, содержащий набор данных со значениями весов синапсов группы.","dynamics_params","Необязательные динамические атрибуты.","Параметры конфигурации нейронной сети прописываются в JSON-файлах. Подробнее о формате SONATA, см. документацию SONATA.","Форматы описания нейронной сети","235878.htm");
Page[65]=new Array("Этот раздел содержит пошаговые инструкции по установке и удалению Kaspersky Neuromorphic Platform.","Для разработки прикладных решений вы можете установить платформу следующими способами:","Установить deb-пакеты.","Установить пакеты для разработки прикладных решений на языке программирования Python.","Вы также можете загрузить архив с исходным кодом платформы и собрать проект для разработки прикладных решений на C++. ","Установка и удаление платформы","235045.htm");
Page[66]=new Array("Раздел находится в разработке.","Установка deb-пакетов","273773.htm");
Page[67]=new Array("Раздел находится в разработке.","Установка пакетов для разработки на языке программирования Python","273774.htm");
Page[68]=new Array("Перед сборкой проекта для разработки прикладных решений необходимо скачать и распаковать архив с исходным кодом платформы.","Чтобы загрузить и распаковать архив с исходным кодом платформы:","Скачайте архив с исходным кодом с репозитория платформы.","Распакуйте в рабочую директорию архив платформы.","Например, вы можете распаковать архив платформы с помощью следующей команды:","tar xf &lt;путь к архиву платформы&gt; --directory &lt;путь к рабочей директории&gt;","Загрузка и распаковка архива с исходным кодом платформы","235046.htm");
Page[69]=new Array("Развернуть все&nbsp;|&nbsp;Свернуть все","После загрузки и распаковки архива с исходным кодом платформы вы можете собрать проект для разработки прикладных решений на языке программирования C++ с помощью Kaspersky Neuromorphic Platform. Для этого необходимо указать параметры сборки проекта в корневом файле скрипта сборки CMakeLists.txt. ","Корневой файл CMakeLists.txt должен содержать следующие команды:","cmake_minimum_required(VERSION 3.25)&nbsp;&#8211; указание минимальной поддерживаемой версии CMake.","Для сборки проекта на базе Kaspersky Neuromorphic Platform требуется CMake версии не ниже 3.25.","project(&lt;название проекта&gt;)&nbsp;&#8211; указание параметров проекта.","set(CMAKE_CXX_STANDARD 17)&nbsp;&#8211; указание C++ 17 в качестве используемого языкового стандарта. ","add_subdirectory(&lt;имя директории с исходным кодом платформы&gt;)&nbsp;&#8211; указание директории, включающая программу, сборку которых требуется выполнить.","add_executable(\"${PROJECT_NAME}\" &lt;файл программы&gt;)&nbsp;&#8211; указание исполняемого файла. ","target_link_libraries(\"${PROJECT_NAME}\" PRIVATE &lt;подключаемые библиотеки платформы&gt;)&nbsp;&#8211; определение библиотек, которые будут подключены к вашему проекту. ","Подробнее о сборке проектов с помощью CMake см. документацию CMake.","Чтобы собрать проект для разработки прикладных решений на C++:","Перейдите в рабочую директорию.","В рабочей директории создайте файл скрипта сборки CMakeLists.txt. ","Откройте созданный файл в любом текстовом редакторе и укажите параметры сборки проекта.","Примеры файла CMakeLists.txt:","Пример файла CMakeLists.txt при загрузке динамических библиотек бекенда","CMakeLists.txt","cmake_minimum_required(VERSION 3.25)","// simple-network&nbsp;&#8211; название проекта","project(simple-network)"," ","set(CMAKE_CXX_STANDARD 17)"," ","// Запускает CMake в директории KNP и добавляет все проекты из директории в сборку","add_subdirectory(KNP)"," ","// Добавляет бинарный файл с исходным кодом в файл программы main.cpp","add_executable(\"${PROJECT_NAME}\" main.cpp)","// Подключает фреймворк к проекту simple-network ","target_link_libraries(\"${PROJECT_NAME}\" PRIVATE KNP::BaseFramework::Core)","Пример файла CMakeLists.txt без загрузки динамических библиотек бекенда","При сборке проекта без загрузки динамических библиотек требуется подключить библиотеки нужных бекендов с помощью метода target_link_libraries. Перечислите необходимые библиотеки через пробел как в приведенном примере.","CMakeLists.txt","cmake_minimum_required(VERSION 3.25)","// digits-recognition&nbsp;&#8211; название проекта","project(digits-recognition)"," ","set(CMAKE_CXX_STANDARD 17)"," ","// Запускает CMake в директории KNP и добавляет все проекты из директории в сборку","add_subdirectory(KNP)"," ","// Добавляет бинарный файл с исходным кодом в файл программы main.cpp","add_executable(\"${PROJECT_NAME}\" main.cpp)","// Подключает фреймворк и библиотеку однопоточного бекенда для CPU к проекту digits-recognition. ","target_link_libraries(\"${PROJECT_NAME}\" PRIVATE KNP::BaseFramework::Core KNP::Backends::CPUSingleThreaded)","Сохраните изменения в файле CMakeLists.txt.","Сборка проекта для разработки решений на C++","255715.htm");
Page[70]=new Array("В зависимости от способа установки вы можете удалить платформу следующими способами:","Удалить deb-пакеты.","Удалить пакеты для разработки прикладных решений на языке программирования Python.","Удалить директорию, в которую был распакован архив с исходным кодом платформы.","Удаление платформы","255720.htm");
Page[71]=new Array("Вы можете участвовать в разработке Kaspersky Neuromorphic Platform. Для этого необходимо получить исходный код с репозитория платформы.","Чтобы участвовать в разработке платформы:","Откройте терминал и с помощью команды cd перейдите в рабочую директорию.","Вы можете использовать любой другой инструмент для работы с Git. ","Для получения исходного кода платформы требуется выполнить следующие действия:","В командной строке выполните следующую команду для загрузки репозитория платформы на устройство:","git clone --recurse-submodules --remote-submodules &lt;репозиторий платформы&gt;","где:","--recurse-submodules означает, что каждый подмодуль в репозитории платформы, включая вложенные подмодули, будет автоматически инициализирован и обновлен.","--remote-submodules означает, что для обновления подмодуля все клонированные подмодули будут использовать статус ветки его удаленного отслеживания.","&lt;репозиторий платформы&gt; соответствует веб-адресу репозитория платформы, полученному от сотрудников \"Лаборатории Касперского\".","Настройте зависимости для работы с исходным кодом платформы.","Если требуется, загрузите удаленный репозиторий платформы в ваш локальный репозиторий. Для этого вы можете использовать команду git pull. ","В командной строке выполните следующую команду для создания ветки, в которой вы будете вносить изменения в код платформы:","git checkout -b &lt;название ветки&gt;","Внесите изменения в код платформы.","Зафиксируйте изменения. ","Например, вы можете использовать следующие команды для публикации изменений: ","git add &lt;имя файла для публикации&gt;","git commit -m \"&lt;сообщение для публикации&gt;\"","git push origin &lt;название ветки&gt;","Опубликуйте изменения в репозитории платформы.","Участие в разработке платформы","255811.htm");
Page[72]=new Array("Условия использования Kaspersky Neuromorphic Platform&nbsp;&#8211; это обязательное соглашение между вами и АО \"Лаборатория Касперского\", в котором изложены условия, на которых вы можете пользоваться платформой.","Внимательно ознакомьтесь с Условиями использования перед началом работы с Kaspersky Neuromorphic Platform.","Вы можете ознакомиться с Условиями использования, прочитав документ LICENSE.txt, расположенном в репозитории платформы.","Если вы не согласны с Условиями использования, вы должны удалить и не использовать Kaspersky Neuromorphic Platform.","Лицензирование платформы","235890.htm");
Page[73]=new Array("Kaspersky Neuromorphic Platform не запрашивает, не хранит и не обрабатывает информацию, относящуюся к персональным данным.","О предоставлении данных","235889.htm");
Page[74]=new Array("Этот раздел содержит описание типовых пользовательских задач, выполняемых на языке программирования C++. ","Решение типовых задач на C++","272341.htm");
Page[75]=new Array("На текущий момент Kaspersky Neuromorphic Platform поддерживает наборы свойств и типизацию популяций по моделям нейронов BLIFATNeuron и SynapticResourceSTDPBLIFATNeuron. Если требуется, вы можете добавить новый тип нейрона.","Вы можете использовать эту инструкцию при работе с исходным кодом платформы.","Чтобы добавить новый тип нейрона:","В директории neuron-trait-library/include/knp/neuron-traits/ создайте заголовочный файл для нового типа нейрона. ","В созданном заголовочном файле определите структуру типа нейрона и его шаблонные свойства (например, default_values, neuron_parameters).","Пример определения BLIFAT-нейрона:","neuron-trait-library/include/knp/neuron-traits/blifat.h","#include \"type_traits.h\""," ","namespace knp::neuron_traits"," ","{","struct BLIFATNeuron;"," ","template &lt;&gt;","struct default_values&lt;BLIFATNeuron&gt;"," ","{","    // Задает количество шагов нейронной сети с шага последнего спайка по умолчанию.","    constexpr static std::size_t n_time_steps_since_last_firing_ = std::numeric_limits&lt;std::size_t&gt;::infinity();","    // Задает значение по умолчанию, к которому стремится мембранный потенциал для тормозных синапсов, основанных на проводимости.","    constexpr static double reverse_inhibitory_potential = -0.3;","    // Задает значение по умолчанию, к которому стремится мембранный потенциал для тормозных синапсов, основанных на токе.","    constexpr static double min_potential = -1.0e9;","    ...","};","template &lt;&gt;","struct neuron_parameters&lt;BLIFATNeuron&gt;","{","    // Задает количество шагов нейронной сети с шага последнего спайка.","    std::size_t n_time_steps_since_last_firing_ = default_values&lt;BLIFATNeuron&gt;::n_time_steps_since_last_firing_;","    // Задает пороговое значение мембранного потенциала нейрона.","    double activation_threshold_ = default_values&lt;BLIFATNeuron&gt;::activation_threshold_;","    // Задает динамическое пороговое значение мембранного потенциала, после достижения которого нейрон генерирует спайк.","    double dynamic_threshold_ = default_values&lt;BLIFATNeuron&gt;::dynamic_threshold_;","    ...","};","}  // namespace knp::neuron_traits","Добавьте созданный тип нейрона в список нейронов, определенных в заголовочном файле neuron-trait-library/include/knp/neuron-traits/all_traits.h.","Пример добавления BLIFAT-нейрона в список нейронов:","neuron-trait-library/include/knp/neuron-traits/all_traits.h","#include \"blifat.h\""," ","namespace knp::neuron_traits"," ","{","// Список типов нейронов, разделенных запятыми.","#define ALL_NEURONS BLIFATNeuron","...","}  // namespace knp::neuron_traits","Реализуйте в нужных бекендах перегруженный метод запуска. ","Например, для бекенда CPU реализуйте перегруженный метод запуска расчета популяций.","Добавление нового типа нейрона","250649.htm");
Page[76]=new Array("На текущий момент Kaspersky Neuromorphic Platform поддерживает наборы свойств и типизацию проекций по моделям синапсов DeltaSynapse, AdditiveSTDPDeltaSynapse и SynapticResourceSTDPDeltaSynapse. Если требуется, вы можете добавить новый тип синапса.","Вы можете использовать эту инструкцию при работе с исходным кодом платформы.","Чтобы добавить новый тип синапса:","В директории synapse-traits-library/include/knp/synapse-traits/ создайте заголовочный файл для нового типа синапса. ","В созданном заголовочном файле определите структуру типа синапса и его шаблонные свойства (например, synapse_parameters).","Пример определения дельта-синапса:","synapse-traits-library/include/knp/synapse-traits/delta.h","#include \"type_traits.h\""," ","namespace knp::synapse_traits"," ","{","struct DeltaSynapse;","template &lt;&gt;","struct default_values&lt;DeltaSynapse&gt;","{","    // Задает значение веса синапса по умолчанию.","    constexpr static float weight_ = 0.0F;","    // Задает значение задержки синапса по умолчанию.","    constexpr static uint32_t delay_ = 1;","    // Задает тип синапса по умолчанию.","    constexpr static OutputType output_type_ = OutputType::EXCITATORY;","};"," ","template &lt;&gt;","struct synapse_parameters&lt;DeltaSynapse&gt;"," ","{","    // Задает атрибуты синапса.","    synapse_parameters() : weight_(0.0F), delay_(1), output_type_(knp::synapse_traits::OutputType::EXCITATORY) {}","    synapse_parameters(float weight, uint32_t delay, knp::synapse_traits::OutputType type)","        : weight_(weight), delay_(delay), output_type_(type)"," ","    // Вес синапса","    float weight_;","    // Задержка синапса","    std::size_t delay_;","    // Тип синапса на выходе","    knp::synapse_traits::OutputType output_type_;","};"," ","}  // namespace knp::synapse_traits","Добавьте созданный тип синапса в список синапсов, определенных в заголовочном файле synapse-traits-library/include/knp/synapse-traits/all_traits.h.","Пример добавления дельта-синапса в список синапсов:","synapse-traits-library/include/knp/synapse-traits/all_traits.h","#include \"delta.h\""," ","namespace knp::synapse_traits"," ","{","// Список типов синапсов, разделенных запятыми.","#define ALL_SYNAPSES DeltaSynapse"," ","...","}  // namespace knp::neuron_traits","Реализуйте в нужных бекендах перегруженный метод запуска. ","Например, для бекенда CPU реализуйте перегруженный метод запуска расчета проекций.","Добавление нового типа синапса","250824.htm");
Page[77]=new Array("Этот раздел содержит инструкции по созданию нейронной сети, ее автоматической загрузке на бекенд и исполнению.","Вы можете использовать эту инструкцию при разработке прикладных решений.","Чтобы автоматически загрузить нейронную сеть на бекенд и исполнить ее:","В директории вашего проекта создайте файл программы формата CPP, в котором будет реализована функция для создания и запуска нейронной сети.","В файле программы подключите заголовочные файлы, необходимые для исполнения нейронной сети, с помощью директивы #include. ","Если требуется, определите псевдонимы с помощью оператора using.","Пример подключения заголовочных файлов для исполнения нейронной сети с популяцией BLIFAT-нейронов и проекцией дельта-синапсов на однопоточном бекенде для CPU:","C++","#include &lt;knp/framework/io/out_converters/convert_set.h&gt;","#include &lt;knp/framework/model_executor.h&gt;","#include &lt;knp/framework/network.h&gt;","#include &lt;knp/neuron-traits/blifat.h&gt;","#include &lt;knp/synapse-traits/delta.h&gt;"," ","#include &lt;filesystem&gt;","  "," ","using DeltaProjection = knp::core::Projection&lt;knp::synapse_traits::DeltaSynapse&gt;;","using BLIFATPopulation = knp::core::Population&lt;knp::neuron_traits::BLIFATNeuron&gt;;","Реализуйте генератор синапсов с нужными вам свойствами или используйте готовый генератор из библиотеки фреймворка.","Пример реализации генератора дельта-синапсов:","C++","// Реализована функция, генерирующая синапсы для проекции, которая будет связана ","// с каналом ввода. Функция хранится в переменной input_projection_gen.","inline std::optional&lt;DeltaProjection::Synapse&gt; input_projection_gen(size_t /*index*/) ","{","    return DeltaProjection::Synapse{{1.0, 1, knp::synapse_traits::OutputType::EXCITATORY}, 0, 0};","}"," "," ","// Реализована функция, генерирующая синапсы для проекции, которая будет замыкать вывод ","// популяции на себя. Функция хранится в переменной synapse_generator.","inline std::optional&lt;DeltaProjection::Synapse&gt; synapse_generator(size_t /*index*/)  ","{","    return DeltaProjection::Synapse{{1.0, 6, knp::synapse_traits::OutputType::EXCITATORY}, 0, 0};","}","Реализуйте генератор нейронов с нужными вам свойствами или используйте готовый генератор из библиотеки фреймворка.","Пример реализации генератора BLIFAT-нейронов:","C++","// Реализована функция, генерирующая нейроны. ","// Функция хранится в переменной neuron_generator.","inline knp::neuron_traits::neuron_parameters&lt;knp::neuron_traits::BLIFATNeuron&gt; neuron_generator(size_t)  ","{","    return knp::neuron_traits::neuron_parameters&lt;knp::neuron_traits::BLIFATNeuron&gt;{};","}","Создайте функцию, в которой будут созданы объекты, необходимые для исполнения нейронной сети.","Пример создания функции main:","C++","main(int argc, const char* const argv[])","{","    ...","В созданной функции создайте объект популяции и передайте в конструктор генератор нейронов.","Пример создания объекта популяции:","C++","    ...","    // Создает объект популяции с одним BLIFAT-нейроном","    BLIFATPopulation population{neuron_generator, 1};","    ","    // Сохраняет UID популяции в переменной output_uid ","    knp::core::UID output_uid = population.get_uid();","    ...","В созданной функции создайте объект проекции, который будет замыкать вывод популяции на себя. Передайте в конструктор идентификатор связанной популяции и генератор синапсов.","Пример создания проекции, замыкающей вывод популяции на себя:","C++","    ...","    // Создает объект проекции с одним дельта-синапсом, замыкающей вывод популяции ","    // самой на себя","    DeltaProjection loop_projection =","            DeltaProjection{population.get_uid(), population.get_uid(), synapse_generator, 1};","    ...","В созданной функции создайте объект входной проекции. Передайте в конструктор генератор синапсов и идентификатор связанной популяции.","Пример создания входной проекции, связанной с каналом ввода и популяцией:","C++","    ...","    // Создает объект входной проекции с одним дельта-синапсом, которой присваивается ","    // нулевой идентификатор(knp::core::UID{false}). Проекция принимает спайки от ","    // канала ввода и отправляет синаптическое воздействие в объект популяции.","    DeltaProjection input_projection = DeltaProjection{knp::core::UID{false}, population.get_uid(), input_projection_gen, 1};","    ","    // Сохраняет UID входной проекции в переменной input_uid","    knp::core::UID input_uid = input_projection.get_uid();","    ...","В созданной функции создайте объект нейронной сети и передайте в него созданные проекции и популяцию.","Пример создания объекта нейронной сети:","C++","    ...","    // Создает объект сети network","    knp::framework::Network network;"," ","    // Добавляет созданный объект популяции population в объект нейронной сети network","    network.add_population(std::move(population));"," ","    // Добавляет созданный объект входной проекции input_projection в объект ","    // нейронной сети network","    network.add_projection(std::move(input_projection));"," ","    // Добавляет созданный объект проекции loop_projection, замыкающей вывод ","    // популяции самой на себя, в объект нейронной сети network","    network.add_projection(std::move(loop_projection));","    ...","В созданной функции определите идентификаторы каналов ввода и вывода.","Пример определения идентификаторов каналов ввода и вывода:","C++","    ...","    // Создает произвольные идентификаторы i_channel_uid и o_channel_uid для каналов ввода и вывода","    knp::core::UID i_channel_uid, o_channel_uid;","    ...","В созданной функции создайте объект модели. Передайте в модель объект нейронной сети, идентификаторы каналов, популяции и входной проекции.","Пример создания модели:","C++","    ...","    // Создает объект модели model и передает в нее объект нейронной сети network","    knp::framework::Model model(std::move(network));"," ","    // Передает в объект модели model созданный идентификатор канала ввода ","    // i_channel_uid и идентификатор входной проекции input_uid.","    model.add_input_channel(i_channel_uid, input_uid);"," ","    // Передает в объект модели model созданный идентификатор канала вывода ","    // o_channel_uid и идентификатор популяции output_uid.","    model.add_output_channel(o_channel_uid, output_uid);","    ...","В созданной функции создайте функтор генерации спайков.","Пример создания функтора генерации спайков:","C++","    ...","    auto input_gen = [](knp::core::messaging::Step step) -&gt; knp::core::messaging::SpikeData","    {","        // Посылает спайки на шагах 0, 5, 10 и 15 исполнения нейронной сети","        if (step % 5 == 0)","        {","            knp::core::messaging::SpikeData s;","            s.push_back(0);","            return s;","        }","        return knp::core::messaging::SpikeData();","    };","    ...","В созданной функции укажите путь к экземпляру бекенда, который будет исполнять нейронную есть. ","Путь к бекенду должен соответствовать пути к динамической библиотеке бекенда. Если экземпляр бекенда находится не по указанному пути, измените путь к бекенду. ","Пример:","C++","    ...","    // Заданный путь приведен в качестве примера. Укажите путь к нужной","    // динамической библиотеке бекенда на вашем локальном компьютере.","    auto backend_path = std::filesystem::path(argv[0]).parent_path().parent_path() / \"lib\" / \"knp-cpu-single-threaded-backend\";","    ...","В созданной функции создайте объект исполнителя модели. Передайте исполнителю модели объект модели, путь к бекенду, идентификатор канала ввода и функтор генерации спайков. Запустите исполнение модели.","Пример создания объекта исполнителя модели и исполнения 20 шагов нейронной сети:","C++","    ...","    // Создает объект исполнителя модели me. Передает в объект me объект модели model, ","    // путь к бекенду backend_path, идентификатор канала ввода i_channel_uid и ","    // функтор генерации спайков input_gen.","    knp::framework::ModelExecutor me(model, backend_path, {{i_channel_uid, input_gen}});"," ","    // Инициализирует исполнитель модели me","    me.init();"," ","    // Получает ссылку на объект канала вывода out_channel из ","    // исполнителя моделей me по идентификатору канала вывода o_channel_uid","    auto &amp;out_channel = me.get_output_channel(o_channel_uid);"," ","    // Запускает исполнение модели на 20 шагов","    me.start([](size_t step) { return step &lt; 20; });","    ...","Шаги исполнения нейронной сети","Исполнение нейронной сети производится циклично. Действия, описанные в цикле, повторяются каждые 5 шагов исполнения сети.","На первом шаге цикла канал ввода посылает спайки входной проекции.","На следующем шаге цикла входная проекция вычисляет синаптические воздействия и отправляет их популяции. После получения синаптического воздействия популяция отправляет спайки каналу вывода и проекции, замыкающей вывод популяции на себя. ","Через 6 шагов цикла после получения спайков, отправленных входной проекцией, проекция, замыкающая вывод популяции на себя, вычисляет синаптические воздействия, и передает воздействия популяции. ","На рисунке ниже представлена схема исполнения нейронной сети.","Схема исполнения нейронной сети","В этом примере нейронная сеть исполняется в рамках цикла со следующими шагами: ","Шаг 0. Канал ввода посылает спайки входной проекции. Шаг повторяется каждые 5 шагов цикла исполнения нейронной сети.","Шаг 1. Входная проекция вычисляет синаптические воздействия и отправляет их популяции. После получения синаптического воздействия популяция отправляет спайки каналу вывода и проекции, замыкающей вывод популяции на себя. Шаг повторяется каждые 5 шагов исполнения сети. ","Шаг 7. Проекция, замыкающая вывод популяции на себя, вычисляет синаптические воздействия после получения спайков, отправленных 6 шагов назад, и передает воздействия популяции. Популяция отправляет спайки каналу вывода и проекции, замыкающей вывод популяции на себя. Шаг повторяется каждые 5 шагов исполнения нейронной сети.","Шаг 13. Проекция, замыкающая вывод популяции на себя, вычисляет синаптические воздействия после получения спайков, отправленных 6 шагов назад, и передает воздействия популяции. Популяция отправляет спайки каналу вывода и проекции, замыкающей вывод популяции на себя. Шаг повторяется каждые 5 шагов исполнения нейронной сети.","Шаг 19. Проекция, замыкающая вывод популяции на себя, вычисляет синаптические воздействия после получения спайков, отправленных на шаге 13, и передает воздействия популяции. Популяция отправляет спайки каналу вывода и проекции, замыкающей вывод популяции на себя. ","При необходимости создайте вектор, в который будут записываться индексы шагов, на которых на канал вывода приходят спайки. ","Пример записи результатов работы модели:","C++","    // Создает вектор results, в который будут записываться индексы шагов со спайками","    std::vector&lt;knp::core::messaging::Step&gt; results;"," ","    // Обновляет канал вывода","    const auto &amp;spikes = out_channel.update();"," ","    // Выделяет в памяти область для спайков","    results.reserve(spikes.size());"," ","    // Записывает в вектор results индексы шагов, на которых на канал вывода ","    // приходят спайки","    std::transform(","        spikes.cbegin(), spikes.cend(), std::back_inserter(results),","        [](const auto &amp;spike_msg) { return spike_msg.header_.send_time_; });"," ","    // Выводит через пробел индексы шагов, на которых на канал вывода приходят спайки","    for (const auto &amp;s : results) std::cout &lt;&lt; s &lt;&lt; \" \";","    // Выводит символ новой строки и вызывает метод flush()","    std::cout &lt;&lt; std::endl;","}","Исполнение нейронной сети, загруженной на бекенд автоматически","256877.htm");
Page[78]=new Array("Этот раздел содержит инструкции по созданию нейронной сети из проекций и популяций, загрузке проекций и популяций на бекенд и исполнению нейронной сети.","Этот вариант построения и исполнения нейронной сети используется редко. <br>Вы можете использовать эту инструкцию при разработке прикладных решений.","Чтобы создать нейронную сеть вручную и исполнить ее:","В директории вашего проекта создайте файл программы формата CPP, в котором будет реализована функция для создания и запуска нейронной сети.","В файле программы подключите заголовочные файлы, необходимые для исполнения нейронной сети, с помощью директивы #include. ","Если требуется, определите псевдонимы с помощью оператора using.","Пример подключения заголовочных файлов для исполнения нейронной сети с популяцией BLIFAT-нейронов и проекцией дельта-синапсов на однопоточном бекенде для CPU:","C++","#include &lt;knp/backends/cpu-single-threaded/backend.h&gt;","#include &lt;knp/core/population.h&gt;","#include &lt;knp/core/projection.h&gt;","#include &lt;knp/neuron-traits/blifat.h&gt;","#include &lt;knp/synapse-traits/delta.h&gt;"," ","#include &lt;vector&gt;"," "," ","using Backend = knp::backends::single_threaded_cpu::SingleThreadedCPUBackend;","using DeltaProjection = knp::core::Projection&lt;knp::synapse_traits::DeltaSynapse&gt;;","using BLIFATPopulation = knp::core::Population&lt;knp::neuron_traits::BLIFATNeuron&gt;;","using Population = knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::PopulationVariants;","using Projection = knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::ProjectionVariants;","Реализуйте генератор нейронов с нужными вам свойствами или используйте готовый генератор из библиотеки фреймворка.","Пример реализации генератора BLIFAT-нейронов:","C++","...","// Реализована функция, генерирующая нейроны. ","// Функция хранится в переменной neuron_generator.","auto neuron_generator = [](size_t index)","{","    return knp::neuron_traits::neuron_parameters&lt;knp::neuron_traits::BLIFATNeuron&gt;{};","};","...","Реализуйте генератор синапсов с нужными вам свойствами или используйте готовый генератор из библиотеки фреймворка.","Пример реализации генератора дельта-синапсов:","C++","...","// Реализована функция, генерирующая синапсы для проекции, которая будет связана ","// с каналом ввода. Функция хранится в переменной input_projection_gen.","DeltaProjection::SynapseGenerator input_projection_gen = [](size_t index) -&gt; std::optional&lt;DeltaProjection::Synapse&gt;","{","    return DeltaProjection::Synapse{{1.0, 1, knp::synapse_traits::OutputType::EXCITATORY}, 0, 0};","};"," ","// Реализована функция, генерирующая синапсы для проекции, которая будет замыкать ","// вывод популяции на себя. Функция хранится в переменной synapse_generator.","DeltaProjection::SynapseGenerator synapse_generator = [](size_t index) -&gt; std::optional&lt;DeltaProjection::Synapse&gt;","{","    return DeltaProjection::Synapse{{1.0, 6, knp::synapse_traits::OutputType::EXCITATORY}, 0, 0};","};","...","Создайте объект популяции и передайте в конструктор созданный генератор нейронов. ","Пример создания объекта популяции:","C++","...","// Создает объект популяции","BLIFATPopulation population{neuron_generator, 1};","Создайте объект проекции, которая будет замыкать вывод популяции на себя. Передайте в конструктор идентификатор связанной популяции и генератор синапсов.","Пример создания проекции, замыкающей вывод популяции на себя:","C++","...","// Создает объект проекции, замыкающей вывод популяции на себя","Projection loop_projection = DeltaProjection{population.get_uid(), population.get_uid(), synapse_generator, 1};","...","Создайте объект входной проекции. Передайте в конструктор генератор синапсов и идентификатор связанной популяции.","Пример создания входной проекции, связанной с каналом ввода и популяцией:","C++","...","// Создает объект входной проекции, которой присваивается нулевой идентификатор ","// (knp::core::UID{false}). Проекция принимает спайки от канала ввода и отправляет ","// синаптическое воздействие в объект популяции.","Projection input_projection = DeltaProjection{knp::core::UID{false}, population.get_uid(), input_projection_gen, 1};","// Получает UID входной проекции, которая отправляет популяции синаптическое воздействие","knp::core::UID input_uid = std::visit([](const auto &amp;proj) { return proj.get_uid(); }, input_projection);","...","Загрузите популяцию и проекции на бекенд.","Пример загрузки популяции и проекций на бекенд:","C++","...","Backend backend;","...","// Загружает на бекенд созданный объект популяции","backend.load_populations({population});","// Загружает на бекенд созданные объекты проекций","backend.load_projections({input_projection, loop_projection});","Определите объект точки подключения.","Пример создания точки подключения:","C++","...","// Создает точку подключения","auto endpoint = backend.message_bus_.create_endpoint();","...","Подключите канал ввода к входной проекции, а также популяцию к каналу вывода. Задайте для каналов ввода и вывода уникальные идентификаторы и сформируйте для них сообщения.","Пример подключения к каналам ввода и вывода:","C++","...","// Создает произвольный UID канала ввода","knp::core::UID in_channel_uid;"," ","// Создает произвольный UID канала вывода","knp::core::UID out_channel_uid;"," ","// Создает подписку на спайки от канала ввода","backend.subscribe&lt;knp::core::messaging::SpikeMessage&gt;(input_uid, {in_channel_uid});"," ","// Создает подписку на спайки от популяции с заданным UID для канала вывода ","// с заданным UID","endpoint.subscribe&lt;knp::core::messaging::SpikeMessage&gt;(out_channel_uid, {population.get_uid()});","...","Настройте передачу сообщений в нейронной сети и укажите параметры исполнения такта нейронной сети.","Следующий пример исполнения нейронной сети состоит из 20 шагов:","C++","...","std::vector&lt;size_t&gt; results;","for (size_t step = 0; step &lt; 20; ++step)","{","    // Посылает спайки входной проекции от канала ввода на шагах 0, 5, 10 и 15","    if (step % 5 == 0)","    {","        knp::core::messaging::SpikeMessage message{{in_channel_uid, 0}, {0}};","        endpoint.send_message(message);","    }","    backend.step();","    endpoint.receive_all_messages();","    // Получает спайки, отправленные популяцией на канал вывода","    auto output = endpoint.unload_messages&lt;knp::core::messaging::SpikeMessage&gt;(out_channel_uid);"," ","    // Записывает индексы шагов, на которых на канал вывода приходят спайки","    if (!output.empty()) results.push_back(step);","}","...","Шаги исполнения нейронной сети","Исполнение нейронной сети производится циклично. Действия, описанные в цикле, повторяются каждые 5 шагов исполнения сети.","На первом шаге цикла канал ввода посылает спайки входной проекции.","На следующем шаге цикла входная проекция вычисляет синаптические воздействия и отправляет их популяции. После получения синаптического воздействия популяция отправляет спайки каналу вывода и проекции, замыкающей вывод популяции на себя. ","Через 6 шагов цикла после получения спайков, отправленных входной проекцией, проекция, замыкающая вывод популяции на себя, вычисляет синаптические воздействия, и передает воздействия популяции. ","На рисунке ниже представлена схема исполнения нейронной сети.","Схема исполнения нейронной сети","В этом примере нейронная сеть исполняется в рамках цикла со следующими шагами: ","Шаг 0. Канал ввода посылает спайки входной проекции. Шаг повторяется каждые 5 шагов цикла исполнения нейронной сети.","Шаг 1. Входная проекция вычисляет синаптические воздействия и отправляет их популяции. После получения синаптического воздействия популяция отправляет спайки каналу вывода и проекции, замыкающей вывод популяции на себя. Шаг повторяется каждые 5 шагов исполнения сети. ","Шаг 7. Проекция, замыкающая вывод популяции на себя, вычисляет синаптические воздействия после получения спайков, отправленных 6 шагов назад, и передает воздействия популяции. Популяция отправляет спайки каналу вывода и проекции, замыкающей вывод популяции на себя. Шаг повторяется каждые 5 шагов исполнения нейронной сети.","Шаг 13. Проекция, замыкающая вывод популяции на себя, вычисляет синаптические воздействия после получения спайков, отправленных 6 шагов назад, и передает воздействия популяции. Популяция отправляет спайки каналу вывода и проекции, замыкающей вывод популяции на себя. Шаг повторяется каждые 5 шагов исполнения нейронной сети.","Шаг 19. Проекция, замыкающая вывод популяции на себя, вычисляет синаптические воздействия после получения спайков, отправленных на шаге 13, и передает воздействия популяции. Популяция отправляет спайки каналу вывода и проекции, замыкающей вывод популяции на себя. ","Исполнение нейронной сети, созданной из проекций и популяций","245069.htm");
Page[79]=new Array("Этот раздел содержит описание типовых пользовательских задач, выполняемых на языке программирования Python. ","Решение типовых задач на Python","272352.htm");
Page[80]=new Array("Этот раздел содержит инструкции по созданию нейронной сети, ее загрузке на бекенд и исполнению.","Вы можете использовать эту инструкцию для разработки прикладных решений.","Чтобы автоматически загрузить нейронную сеть на бекенд и исполнить ее:","В директории вашего проекта создайте файл программы с расширением .py, в котором будет реализована функция для создания и запуска нейронной сети.","В файле программы импортируйте объекты библиотек платформы, необходимые для исполнения нейронной сети.","Если требуется, определите псевдонимы с помощью оператора as.","Пример подключения объектов библиотек для исполнения нейронной сети с популяцией BLIFAT-нейронов и проекцией дельта-синапсов на однопоточном бекенде для CPU:","Python","from knp.base_framework._knp_python_framework_base_framework import BackendLoader","from knp.core._knp_python_framework_core import UID, BLIFATNeuronPopulation, DeltaSynapseProjection, SpikeMessage","from knp.neuron_traits._knp_python_framework_neuron_traits import BLIFATNeuronParameters","from knp.synapse_traits._knp_python_framework_synapse_traits import DeltaSynapseParameters, OutputType","Реализуйте генератор нейронов с нужными вам свойствами или используйте готовый генератор из библиотеки фреймворка.","Пример реализации генератора BLIFAT-нейронов:","Python","# Реализована функция, генерирующая BLIFAT-нейроны","def neuron_generator(_):  # type: ignore[no-untyped-def]","    return BLIFATNeuronParameters()","Реализуйте генератор синапсов с нужными вам свойствами или используйте готовый генератор из библиотеки фреймворка.","Пример реализации генератора дельта-синапсов:","Python","# Реализована функция, генерирующая дельта-синапсы для проекции, которая будет замыкать вывод","# популяции на себя ","def synapse_generator(_):  # type: ignore[no-untyped-def]","    return DeltaSynapseParameters(1.0, 6, OutputType.EXCITATORY), 0, 0","# Реализована функция, генерирующая дельта-синапсы для проекции, которая будет связана ","# с каналом ввода","def input_projection_gen(_):  # type: ignore[no-untyped-def]","    return DeltaSynapseParameters(1.0, 1, OutputType.EXCITATORY), 0, 0","Создайте функцию, в которой будут созданы объекты, необходимые для исполнения нейронной сети.","Пример создания функции main:","Python","def main():  # type: ignore[no-untyped-def]","    ...","В созданной функции создайте объект популяции и передайте в конструктор генератор нейронов.","Пример создания объекта популяции:","Python","    ...","    # Создает объект популяции с одним BLIFAT-нейроном","    population = BLIFATNeuronPopulation(neuron_generator, 1)","    ...","В созданной функции создайте объект проекции, который будет замыкать вывод популяции на себя. Передайте в конструктор идентификатор связанной популяции и генератор синапсов.","Пример создания проекции, замыкающей вывод популяции на себя:","Python","    ...","    # Создает объект проекции с одним дельта-синапсом, замыкающей вывод популяции","    # на себя","    loop_projection = DeltaSynapseProjection(population.uid, population.uid, ","    synapse_generator, 1)","    ...","В созданной функции создайте объект входной проекции. Передайте в конструктор генератор синапсов и идентификатор связанной популяции.","Пример создания входной проекции, связанной с каналом ввода и популяцией:","Python","    ...","    # Создает объект входной проекции с одним дельта-синапсом, которой присваивается","    # нулевой идентификатор (UID(False)). Проекция принимает спайки от канала ввода","    # и отправляет синаптическое воздействие в объект популяции.","    input_projection = DeltaSynapseProjection(UID(False), population.uid, ","    input_projection_gen, 1)","    # Сохраняет UID входной проекции в переменной input_uid","    input_uid = input_projection.uid","    ...","В созданной функции загрузите экземпляр бекенда и загрузите в него созданные популяцию и проекции. Создайте точку подключения.","Пример загрузки бекенда и создания точки подключения:","Python","    ...","    # Загружает экземпляр однопоточного бекенда для CPU с вашего локального компьютера","    backend = BackendLoader().load(f'{pytestconfig.rootdir}/../lib/libknp-cpu-single-","    threaded-backend')","    # Загружает созданную популяцию в экземпляр бекенда","    backend.load_all_populations([population])","    # Загружает созданные проекции в экземпляр бекенда","    backend.load_all_projections([input_projection, loop_projection])","    # Инициализирует бекенд","    backend._init()","    # Создает объект точки подключения","    endpoint = backend.message_bus.create_endpoint()","    ...","В созданной функции определите идентификаторы каналов ввода и вывода.","Пример определения идентификаторов каналов ввода и вывода:","Python","    ...","    # Создает произвольные идентификаторы in_channel_uid и out_channel_uid для каналов ","    # ввода и вывода","    in_channel_uid = UID()","    out_channel_uid = UID()","    ...","В созданной функции подпишите бекенд и точку подключения на сообщения от каналов ввода и вывода.","Пример подписки бекенда и точки подключения на сообщения SpikeMessage:","Python","    ...","    # Подписывает бекенд на спайки от канала ввода","    backend.subscribe(SpikeMessage, input_uid, [in_channel_uid])","    # Подписывает точку подключения на спайки от канала вывода","    endpoint.subscribe(SpikeMessage, out_channel_uid, [population.uid])","    ...","В созданной функции исполните нейронную сеть. При необходимости создайте список, в который будут записываться индексы шагов, на которых на канал вывода приходят спайки.","Пример исполнения 20 шагов нейронной сети:","Python","    ... ","    # Создает список results, в который будут записываться индексы шагов со спайками","    results = [] ","    ","    for step in range(0, 20):","        # Посылает спайки на шагах 0, 5, 10 и 15 исполнения нейронной сети","        if step % 5 == 0:","            # Отображает номер шага, на котором канал ввода отправил спайк входной проекции","            print(f'STEP {step}')","            message = SpikeMessage((in_channel_uid, step), [0])","            endpoint.send_message(message)","        backend._step()","        messages_count = endpoint.receive_all_messages()","        output = endpoint.unload_messages(SpikeMessage, out_channel_uid)","        if len(output):","            # Отображает номер шага, на котором на канал вывода пришел спайк","            print(f'STEP {step}')","Шаги исполнения нейронной сети","Исполнение нейронной сети производится циклично. Действия, описанные в цикле, повторяются каждые 5 шагов исполнения сети.","На первом шаге цикла канал ввода посылает спайки входной проекции.","На следующем шаге цикла входная проекция вычисляет синаптические воздействия и отправляет их популяции. После получения синаптического воздействия популяция отправляет спайки каналу вывода и проекции, замыкающей вывод популяции на себя. ","Через 6 шагов цикла после получения спайков, отправленных входной проекцией, проекция, замыкающая вывод популяции на себя, вычисляет синаптические воздействия, и передает воздействия популяции. ","На рисунке ниже представлена схема исполнения нейронной сети.","Схема исполнения нейронной сети","В этом примере нейронная сеть исполняется в рамках цикла со следующими шагами: ","Шаг 0. Канал ввода посылает спайки входной проекции. Шаг повторяется каждые 5 шагов цикла исполнения нейронной сети.","Шаг 1. Входная проекция вычисляет синаптические воздействия и отправляет их популяции. После получения синаптического воздействия популяция отправляет спайки каналу вывода и проекции, замыкающей вывод популяции на себя. Шаг повторяется каждые 5 шагов исполнения сети. ","Шаг 7. Проекция, замыкающая вывод популяции на себя, вычисляет синаптические воздействия после получения спайков, отправленных 6 шагов назад, и передает воздействия популяции. Популяция отправляет спайки каналу вывода и проекции, замыкающей вывод популяции на себя. Шаг повторяется каждые 5 шагов исполнения нейронной сети.","Шаг 13. Проекция, замыкающая вывод популяции на себя, вычисляет синаптические воздействия после получения спайков, отправленных 6 шагов назад, и передает воздействия популяции. Популяция отправляет спайки каналу вывода и проекции, замыкающей вывод популяции на себя. Шаг повторяется каждые 5 шагов исполнения нейронной сети.","Шаг 19. Проекция, замыкающая вывод популяции на себя, вычисляет синаптические воздействия после получения спайков, отправленных на шаге 13, и передает воздействия популяции. Популяция отправляет спайки каналу вывода и проекции, замыкающей вывод популяции на себя. ","Исполнение нейронной сети, загруженной на бекенд вручную","272357.htm");
Page[81]=new Array("Вы можете использовать этот сценарий при разработке прикладных решений.","Сценарий исполнения нейронной сети из тернарных слоев для классификации изображений состоит из следующих этапов:","Создание и обучение нейронной сети из тернарных слоев","Создайте нейронную сеть с помощью тернарных слоев библиотеки ANN2SNN. В качестве функции активации выходного слоя укажите сигмоидную функцию. Подготовьте данные для обучения нейронной сети. Приведите данные для обучения в бинарный вид. Обучите нейронную сеть и оцените результаты обучения.","Размещение нейронной сети на нейроморфном процессоре Алтай-2","Разместите нейронную сеть на нейроморфном процессоре Алтай-2 с помощью следующей команды:","placer.py -p &lt;путь к файлу с обученной нейронной сетью&gt; -l &lt;путь к файлу, в который будут записываться журналы утилиты placer&gt; -o &lt;путь к конфигурационному файлу для загрузки на нейроморфный процессор&gt;","Например, placer.py -p mnist.h5 -l mnist_placer.log -o mnist_altai.json","Запуск инференса нейронной сети","Укажите конфигурационный файл и компонент для исполнения нейронной сети. Нейронная сеть может быть исполнена как на аппаратном обеспечении нейроморфного процессора Алтай-2, так и на программном эмуляторе. Подготовьте данные для инференса и приведите их в бинарный вид. Запустите инференс.","Сценарий исполнения нейронной сети из тернарных слоев для классификации изображений","273512.htm");
Page[82]=new Array("Этот раздел содержит инструкции по созданию нейронной сети для классификации изображений и ее обучению. Нейронная сеть создана с помощью тернарных слоев библиотеки ANN2SNN. ","Чтобы создать и обучить нейронную сеть из тернарных слоев для классификации изображений:","В директории вашего проекта создайте файл программы с расширением .py, в котором будет создана и обучена нейронная сеть.","В файле программы импортируйте модули библиотек, необходимые для создания и исполнения нейронной сети.","Если требуется, определите псевдонимы с помощью оператора as.","Пример подключения объектов для создания и исполнения сверточной нейронной сети:","Python","from altainn.ternary_tf2.layers import TernaryConv2D, TernaryDense","from tensorflow.keras.models import Sequential","from tensorflow.keras.layers import Flatten, Dense","from altainn.binarynet_tf2.layers import Clip","from altainn.ternary_tf2.ops import heaviside_mod as heaviside","import tensorflow as tf","Создайте нейронную сеть. Укажите сигмоидную функцию в качестве функции активации для выходного слоя.","Пример создания нейронной сети с тремя сверточными слоями:","Python","model = Sequential(","[TernaryConv2D(3, 2, activation=heaviside, input_shape=(28, 28, 1)),","TernaryConv2D(6, 3, activation=heaviside),","TernaryConv2D(6, 3, activation=heaviside),","Flatten(),","TernaryDense(10, activation=\"sigmoid\")])","model.compile(optimizer=\"adam\", loss=\"categorical_crossentropy\", metrics=[\"accuracy\"])","Подготовьте данные для обучения.","Пример подготовки данных MNIST для обучения:","Python","# Загружает данные MNIST для обучения","(x_train, y_train), (x_test, y_test) = tf.keras.datasets.mnist.load_data() ","# Кодирует целевые метки","y_train = tf.keras.utils.to_categorical(y_train) ","y_test = tf.keras.utils.to_categorical(y_test) ","# Нормализует входные данные","x_train = x_train / 255.0 ","x_test = x_test / 255.0","# Приводит входные данные в бинарный вид","x_train[x_train &gt; 0.5] = 1 ","x_train[x_train &lt;= 0.5] = 0","	","x_test[x_test &gt; 0.5] = 1","x_test[x_test &lt;= 0.5] = 0","# Меняет размерность данных из (28, 28) в (28, 28, 1) для правильной работы сверточных слоев	","x_train = x_train.reshape(x_train.shape + (1,)) ","x_test = x_test.reshape(x_test.shape + (1,))","Создайте Callback-функцию для сохранения нейронной сети.","Пример Callback-функции:","Python","#Сохраняет обученную нейронную сеть в файле mnist.h5 ","model_chekpoint_callback = ModelCheckpoint(","    filepath=\"mnist.h5\",","    monitor=\"accuracy\",","    mode=\"max\",","    save_best_only=True","Обучите нейронную сеть.","Пример обучения нейронной сети:","Python","# Запускает обучение на тренировочных данных","model.fit(x_train, y_train, epochs = 15, verbose = 1, callbacks=[model_chekpoint_callback]) ","# Загружает обученную модель","model = load_model(\"mnist.h5\", custom_objects = {\"heaviside_mod\": heaviside, \"TernaryDense\": TernaryDense, ","    \"TernaryConv2D\": TernaryConv2D, \"Clip\": Clip})","#Оценивает точность нейронной сети","print(f\"Точность модели: {model.evaluate(x_test, y_test, verbose = 1)[1]*100:.2f}%\") ","Создание и обучение нейронной сети из тернарных слоев для классификации изображений","273484.htm");
Page[83]=new Array("Этот раздел содержит инструкции по запуску инференса нейронной сети для классификации изображений на нейроморфном процессоре Алтай-2. Нейронную сеть требуется предварительно создать с помощью тернарных слоев библиотеки ANN2SNN, обучить и разместить на нейроморфном процессоре. ","Чтобы запустить инференс нейронной сети из тернарных слоев для классификации изображений:","В директории вашего проекта создайте файл программы с расширением .py, в котором будет реализован инференс нейронной сети.","В файле программы импортируйте модули библиотек, необходимые для инференса нейронной сети.","Если требуется, определите псевдонимы с помощью оператора as.","Пример подключения объектов для инференса нейронной сети:","Python","from python_altai.altai import Altai","import numpy as np","Укажите конфигурационный файл и компонент для исполнения нейронной сети.","Пример конфигурации:","Python","# Для переменной PATH_TO_CONFIGURATION укажите путь к конфигурационному файлу","# для загрузки на нейроморфный процессор","# Для переменной MODE укажите одно из следующих значений:","# hw &#8211; для инференса нейронной сети на аппаратном обеспечении нейроморфного процессора Алтай-2","# gm &#8211; для инференса нейронной сети на программном эмуляторе","PATH_TO_CONFIGURATION = 'mnist_altai.json'","MODE = 'gw'","# Загрузка конфигурации","altai_layer = Altai()","altai_layer.build(PATH_TO_CONFIGURATION, MODE)","Подготовьте данные для инференса.","Пример подготовки данных MNIST для инференса:","Python","# Загружает данные MNIST для инференса","(_, _), (x_test, y_test) = tf.keras.datasets.mnist.load_data() ","# Нормализует входные данные","x_test = x_test / 255.0","# Приводит входные данные в бинарный вид","x_test[x_test &gt; 0.5] = 1","x_test[x_test &lt;= 0.5] = 0","Запустите инференс. ","Пример инференса:","Python","for x in x_test:","    # Преобразует входной вектор в спайки","    altai_layer.prepare_spikes(x)","    # Запускает генерацию сигнала тик","    altai_layer.start_ticks(9)","    # Получает спайки от нейронной сети","    predict = altai_layer.get_spikes()","Запуск инференса нейронной сети из тернарных слоев для классификации изображений","273538.htm");
Page[84]=new Array("Этот раздел содержит инструкции по созданию регрессионной модели с обучаемыми блоками, кодирующие и декодирующие данные, и ее обучению. Модель создана с помощью тернарных слоев библиотеки ANN2SNN. ","Вы можете использовать эту инструкцию при разработке прикладных решений.","Чтобы создать и обучить регрессионную модель:","В директории вашего проекта создайте файл программы с расширением .py, в котором будет создана и обучена модель.","В файле программы импортируйте модули библиотек, необходимые для создания и обучения модели.","Если требуется, определите псевдонимы с помощью оператора as.","Пример подключения объектов для создания и обучения регрессионной модели:","Python","from altainn.ternary_tf2.layers import TernaryDense","from tensorflow.keras.models import Sequential, load_model","from tensorflow.keras.layers import Dense","from altainn.ternary_tf2.ops import heaviside_mod as  heaviside","from altainn.binarynet_tf2.layers import Clip","import matplotlib.pyplot as plt","from tensorflow.keras.callbacks import ModelCheckpoint","import numpy as np","Создайте синусоиду и подготовьте данные для обучения.","Пример подготовки данных: ","Python","# Создает синусоиду","x = np.arange(4000)","x = x * 0.1","arr = np.sin(x)  ","# Берет для обучения 10 значений, по которым прогнозируются следующие значения","# Формирует тренировочные данные","x_train = []","y_train = []","for i in range(0, 3000-11):","    x_train.append(arr[i:i+10])","    y_train.append(arr[i+11])","# Формирует тестовые данные","x_test = []","y_test = []","for i in range(3000, 4000-11):","    x_test.append(arr[i:i+10])","    y_test.append(arr[i+11])","x_train = np.array(x_train)","x_test = np.array(x_test)","y_train = np.array(y_train)","y_test = np.array(y_test)","Создайте регрессионную модель. Для этого создайте кодирующий блок, импульсную нейронную сеть и декодирующий блок. ","Пример создания регрессионной модели:","Python","# Создает кодирующий блок с одним слоем, который преобразует данные в спайки","encoder = Sequential([","    Dense(10, activation=heaviside, input_shape=(10,)),","])","# Создает импульсную нейронную сеть","snn = Sequential([","    TernaryDense(32, activation=heaviside, input_shape=(10,)),","    TernaryDense(32, activation=heaviside),","])","# Создает декодирующий блок с одним слоем, который преобразует спайки в целевое значение","decoder = Sequential([","    Dense(1, activation=\"linear\", input_shape=(32,))","])","# Создает модель с кодирующим блоком, нейронной сетью и декодирующим блоком","model = Sequential([encoder, snn, decoder])","model.compile(optimizer=\"adam\", loss=\"mse\")","Обучите модель.","Пример обучения модели:","Python","# Обучает модель на тренировочных данных","checkpoint = ModelCheckpoint('sin_model.h5', monitor='val_loss', verbose=1, save_best_only=True, mode='min')","model.fit(x_train, y_train, epochs=50, validation_data=(x_test, y_test), callbacks=[checkpoint])"," ","# Загружает модель","model = load_model('sin_model.h5', custom_objects={'heaviside_mod': heaviside, 'Clip': Clip, \"TernaryDense\": TernaryDense})","# Получает предсказания","predict_test = model.predict(x_test)","# Строит графики по предсказанным и целевым значениям","plt.plot(predict_test, 'b')","plt.plot(y_test, 'r', alpha=0.5)","plt.show()","Создание и обучение регрессионной модели","273724.htm");
Page[85]=new Array("В Kaspersky Neuromorphic Platform реализован интерфейс прикладного программирования (Application Programming Interface, API), который обеспечивает доступ к методам платформы. ","@ ОТКРЫТЬ СПРАВОЧНОЕ РУКОВОДСТВО API для фреймворка для C++&nbsp;(на английском языке)","С помощью Kaspersky Neuromorphic Platform API вы можете выполнять следующие действия:","получать данные об используемых устройствах и бекендах;","получать данные об объектах библиотеки поддержки бекендов;","получать данные об объектах фронтенда платформы;","управлять объектами библиотеки поддержки бекендов;","создавать и обучать нейронные сети;","управлять гиперпараметрами нейронных сетей;","создавать новые модели нейронов;","создавать новые топологии нейронных сетей;","реализовывать прикладные решения.","Использование Kaspersky Neuromorphic Platform API","240504.htm");
Page[86]=new Array("Программно-аппаратная часть платформы, обеспечивающая универсальный интерфейс к вычислителю и реализующая функции нейронов и синапсов, а также их модификаторов в терминах вычислителя.","Бекенд","90.htm#o251916");
Page[87]=new Array("Значение сигнала, который синапс передает нейронам связанной популяции.","Вес синапса","90.htm#o251925");
Page[88]=new Array("Устройство или код, непосредственно выполняющий сеть.","Вычислитель","90.htm#o251918");
Page[89]=new Array("Код, изменяющий поведение нейрона или синапса, например добавляющий синаптическую пластичность.","Модификатор","90.htm#o251926");
Page[90]=new Array("Узел нейронной сети, имеющий набор атрибутов. На основе атрибутов нейрона и последовательности входных сигналов происходит вычисление некоторой математической функции, результатом которого является синаптическое воздействие или его отсутствие.","Нейрон","90.htm#o251914");
Page[91]=new Array("Интервал времени между моментом начала Хеббовской пластичности перед первым спайком в ППС и одним из следующих моментов времени: достижение максимально допустимого расстояния (ISImax) после последнего спайка в ППС, форсированное срабатывание или приход спайка на дофаминовый синапс.","Период ППС","90.htm#o264599");
Page[92]=new Array("Последовательность генерируемых нейроном нефорсированных спайков, в которой все соседние спайки разделены временем не большее, чем ISImax, где ISImax&nbsp;&#8211; это максимальное допустимое расстояние между спайками.","Плотная последовательность спайков (ППС)","90.htm#o264601");
Page[93]=new Array("Контейнер, содержащий набор нейронов и их функцию в соответствии с типом нейронов.","Популяция","90.htm#o251919");
Page[94]=new Array("Контейнер, содержащий набор синапсов одного типа, соединяющих нейроны пресинаптической и постсинаптической популяций. ","Проекция","90.htm#o251920");
Page[95]=new Array("Место соединения нейронов, основная функция которого&nbsp;&#8211; передача сигнала от одного нейрона к другому. Каждый нейрон может быть связан с множеством синапсов.","Синапс","90.htm#o251915");
Page[96]=new Array("Значение, формируемое синапсом и предназначенное для изменения значений атрибутов нейронов.","Синаптическое воздействие","90.htm#o251924");
Page[97]=new Array("Связь синапса с пресинаптическим или постсинаптическим нейроном.","Соединение","90.htm#o251929");
Page[98]=new Array("Кратковременный потенциал действия, сгенерированный нейроном популяции в результате изменения атрибутов нейрона.","Спайк","90.htm#o251921");
Page[99]=new Array("Квант времени синхросигнала, за который вычислитель ядра проводит одну операцию.","Такт","90.htm#o273641");
Page[100]=new Array("Квант времени, за который выполняется вычисление функции всех нейронов в ядре. За время тика все спайки, сгенерированные ядрами или поступившие на входные линии нейронов, доставляются до целевых нейронов.","Тик","90.htm#o273599");
Page[101]=new Array("Программно-аппаратная часть платформы, определяющая ее структуру и обеспечивающая управление бекендами. В Kaspersky Neuromorphic Platform реализован фреймворк на языке программирования C++.","Фреймворк","90.htm#o251917");
Page[102]=new Array("Система, описывающая и изменяющая поведение нейрона. Результатом функции является спайк или его отсутствие.","Функция нейрона","90.htm#o251927");
Page[103]=new Array("Система, моделирующая поведение синапса. Результатом функции является синаптическое воздействие или его отсутствие.","Функция синапса","90.htm#o251928");
Page[104]=new Array("Информация о стороннем коде содержится в файле NOTICES.txt, расположенном в репозитории платформы.","Информация о стороннем коде","262312.htm");
Page[105]=new Array("Зарегистрированные товарные знаки и знаки обслуживания являются собственностью их правообладателей.","LTS и Ubuntu&nbsp;являются зарегистрированными товарными знаками Canonical Ltd.","Intel и Core&nbsp;&#8211; товарные знаки Intel Corporation, зарегистрированные в Соединенных Штатах Америки и в других странах.","Linux&nbsp;&#8211; товарный знак Linus Torvalds, зарегистрированный в США и в других странах.","Python &#8211; товарный знак или зарегистрированный товарный знак Python Software Foundation.","Debian&nbsp;&#8211; зарегистрированный товарный знак Software in the Public Interest, Inc.","Уведомления о товарных знаках","240611.htm");
var PageCount=106;
var parsedMainTitle = 'Kaspersky Neuromorphic Platform ';
var PdfOptions = {"pdfName":"../../pdf/OfflineHelp-ru-RU.pdf"};
var reviewDate = 1713513351354;
(function () {
    var maxTextLength = 0;
    var fakeDiv = $('<div/>');
    var text;
    var pageSearch = _.filter(Page, function(page) {
        return page;
    });
    pageSearch = _.map(pageSearch, function (page) {
        return {
            link: page[page.length - 1],
            title: page[page.length - 2],
            text: _.map(page.slice(0, page.length - 2), function(item) {
                text = fakeDiv.html(item).text();
                maxTextLength = Math.max(maxTextLength, text.length);

                return text;
            })
        };
    });

    fakeDiv.remove();

    window.search = function (searchWord, searchWordMinLength) {
        var result = {
            found: false,
            isInit: false,
            results: [],
            resultsCount: 0
        };

        if (searchWord !== '') {
            result.searchWord = searchWord;

            var searchLength = searchWord.length > 5 ? searchWord.length - 2 : searchWordMinLength;

            var fuseOptions = {
                shouldSort: true,
                includeScore: true,
                includeMatches: true,
                minMatchCharLength: searchLength,
                tokenize: true,
                matchAllTokens: true,
                findAllMatches: true,
                threshold: 0.2,
                location: 0,
                distance: maxTextLength,
                keys: [{
                    name: 'title',
                    weight: 0.8
                  }, {
                    name: 'text',
                    weight: 0.2
                  }]
            };


            var fuse = new Fuse(pageSearch, fuseOptions);
            var currentResults = fuse.search(searchWord);

            searchWord = searchWord.replace(/(\\|\[|\*|\+|\(|\)|\.|"|'|`)/g, '\\$1');
            var searchRegExp = new RegExp(searchWord, 'gi');

            if (currentResults.length) {
                result.found = true;
                result.results = _.chain(currentResults)
                    .map(function (result) {
                        var item = result.item;
                        var matches = result.matches.slice(0,2);

                        return {
                            title: item.title.replace(searchRegExp, '<mark>$&</mark>'),
                            link: item.link,
                            texts: _.reduce(matches, function(previous, searchResult) {
                                if (searchResult.key === 'text') {
                                    var originalString = searchResult.value;
                                        var firstEntryPosition = originalString.search(searchRegExp);
                                        if (firstEntryPosition < 0) {
                                            var processedResult = originalString;
                                        } else if (firstEntryPosition < 100) {
                                            var slicePosition = Math.max(30,100-firstEntryPosition);
                                            var sentenceStart = originalString.slice(0,firstEntryPosition);
                                            var contextAfterEntry = originalString.slice(firstEntryPosition, firstEntryPosition + slicePosition);
                                            var afterContext = originalString.slice(firstEntryPosition + slicePosition);
                                            var preResult = sentenceStart + contextAfterEntry + afterContext.slice(0,afterContext.indexOf(' '));
                                            var processedResult = preResult.replace(/[.,:;]$/,"") + '...';
                                        } else {
                                            var beforeFirstEntry = originalString.slice(firstEntryPosition-100,firstEntryPosition);
                                            var startOfSentencePosition = beforeFirstEntry.lastIndexOf('. ');
                                            if (startOfSentencePosition < 0) {
                                                var contextOfEntry = originalString.slice(firstEntryPosition-60,firstEntryPosition+60);
                                                var beforeContext = originalString.slice(0,firstEntryPosition-60);
                                                var afterContext = originalString.slice(firstEntryPosition+60);
                                                var preResult = '...' + beforeContext.slice(beforeContext.lastIndexOf(' ')) + contextOfEntry + afterContext.slice(0,afterContext.indexOf(' '));
                                            } else {
                                                var sentenceContainingEntry = beforeFirstEntry.slice(startOfSentencePosition+2);
                                                var contextAfterEntry = originalString.slice(firstEntryPosition, firstEntryPosition+30);
                                                var afterContext = originalString.slice(firstEntryPosition+30);
                                                var preResult = sentenceContainingEntry + contextAfterEntry + afterContext.slice(0,afterContext.indexOf(' '));
                                            }
                                            var processedResult = preResult.replace(/[.,:;]$/,"") + '...';
                                        }
                                    previous.push(processedResult.replace(searchRegExp, '<mark>$&</mark>'));
                                }
                                return previous;
                            }, [])
                        }
                    })
                    .value();

                result.resultsCount = result.results.length;
            }
        }
        return result;
    };
})();


if (typeof String.prototype.trim !== 'function') {
    String.prototype.trim = function () {
        return this.replace(/^\s+|\s+$/g, '');
    };
}

window.loadParent = function () {};

if (!window.isTOCLoaded) {
    window.isTOCLoaded = function () {};
}

// hide content to avoid blink
$(window.document.documentElement).css('visibility', 'hidden');

$(function () {
    var $aside, $container, $content, $header, $modalContent, $modalHeader, $overlay, $pageTitle,
        $tooltip, $tooltipContent, tooltipButton;

    $.support.cors = true;
    $.fx.speeds._default = 200;

    var options = {
        helpers: {
            media_hv_min: 568,
            media_hv_middle: 874,
            media_mobile: 768
        },
        isMacProject: Boolean(window['isMacProject']),
        isIE7: Boolean(navigator.appVersion.match('MSIE 7.')),
        isIE8: Boolean(navigator.appVersion.match('MSIE 8.')),
        isIE9: Boolean(navigator.appVersion.match('MSIE 9.')),
        isIE10: Boolean(navigator.appVersion.match('MSIE 10.')),
        isFirefox: Boolean(navigator.userAgent.match('Firefox')),
        isOperaMini: Object.prototype.toString.call(window["operamini"]) === "[object OperaMini]",
        isFileProtocol: window.location.protocol === 'file:',
        isAllInOne: window.location.pathname.match(/(all-in-one\.htm)[l]?$/),
        searchWord: getParam('searchWord', true),
        searchWordMinLength: 2
    };

    options.lessThanIE11 = options.isIE7 || options.isIE8 || options.isIE9 || options.isIE10;

    if (window.Customization) {
        _.each(window.Localization, function(item, key) {
            if (_.isObject(window.Customization[key])) {
                window.Localization[key] = window.Customization[key].Value;
            }
        });
    }

    setParam('searchWord', null, true);

    var HELP = window.HELP = $.extend(true, window.HELP, options);
    var $window = $(window);
    var html = document.documentElement;
    var $html = $(html);
    var $head = $(document.head);
    var $body = $(document.body);
    var title = $html.find('title').text();

    $body.removeClass("no-js");

    var ContainerTemplateFunction = _.template(ContainerTemplate);
    var HeaderTemplateFunction = _.template(HeaderTemplate);
    var FooterTemplateFunction = _.template(FooterTemplate);
    var AsideTemplateFunction = _.template(AsideTemplate);
    var SearchResultsTemplateFunction = _.template(SearchResultsTemplate);
    var MobileSearchResultsTemplateFunction = _.template(MobileSearchResultsTemplate);
    var LangListTemplateFunction = _.template(LangListTemplate);
    var VersionsTemplateFunction = _.template(VersionsTemplate);
    var MenuTemplateFunction = _.template(MenuTemplate);
    var AllInOneMenuTemplateFunction = _.template(AllInOneMenuTemplate);
    var CustomStylesTemplateFunction = _.template(CustomStylesTemplate);

    window.parsedMainTitle = window.parsedMainTitle || "Kaspersky Online Help";
    window.toggleBlock = toggleBlock;
    window.showAll = showAll;
    window.hideAll = hideAll;
    window.togglePopup = togglePopup;
    window.showPopup = showTooltip;
    window.hidePopup = hideTooltip;

    function getParam(param, useSession) {
        return store(param, undefined, { useSession: useSession });
    }

    function setParam(param, value, useSession) {
        if (value === void 0) {
            value = null;
        }

        store(param, value, { useSession: useSession });
    }

    function toggleBlock(id) {
        if (!HELP.isAllInOne) {
            var $block = $('#d' + id);
            var $toggler = $('#h' + id);

            if ($block.length > 0 && $toggler.length > 0) {
                $block.slideToggle();
                $toggler.toggleClass('is-expanded');
                $window.resize();
            }
        }
    }

    function showAll() {
        $('.expandingblock').slideDown();
        $('.expandingblocktemplate').addClass('is-expanded');
        $window.resize();
    }

    function hideAll() {
        $('.expandingblock').slideUp();
        $('.expandingblocktemplate').removeClass('is-expanded');
        $window.resize();
    }

    function togglePopup(id, legacyblockname, show) {
        switch (show) {
            case true:
                showTooltip(id, legacyblockname);
                break;
            case false:
                hideTooltip();
                break;
            default:
                toggleTooltip(id, legacyblockname);
        }
    }

    function calcTooltipPosition() {
        if (tooltipButton) {
            var body = document.body;
            var delta = 30;
            var offset = 10;

            if (options.isIE7 || options.isIE8) {
                offset = 0;
            }

            var bodyHeight = body.clientHeight;
            var bodyWidth = body.clientWidth;
            var linkRect = tooltipButton.getBoundingClientRect();
            var tooltip = $tooltip[0];
            var isToTop = linkRect.top > bodyHeight - linkRect.bottom;
            var isToLeft = linkRect.left > bodyWidth - linkRect.right;
            var coords = {
                top: isToTop ? "" : linkRect.bottom + offset,
                bottom: isToTop ? bodyHeight - linkRect.top + offset : "",
                left: isToLeft ? "" : linkRect.left - delta,
                right: isToLeft ? bodyWidth - linkRect.right - delta : ""
            };

            $tooltip.toggleClass("is-to-top", isToTop).toggleClass("is-to-left", isToLeft).css(coords);

            var tooltipRect = tooltip.getBoundingClientRect();
            coords = {};

            var isOverlapLeft = tooltipRect.left < delta;
            var isOverlapRight = tooltipRect.right > bodyWidth - delta;

            if (isOverlapLeft) {
                coords.left = delta;
            }

            if (isOverlapRight) {
                coords.right = delta;
            }

            if (isOverlapLeft || isOverlapRight) {
                coords.width = "auto";
            } else {
                coords.width = "";
            }

            $tooltip.css(coords);
            tooltipRect = tooltip.getBoundingClientRect();
            coords = {};

            var isOverlapTop = tooltipRect.top < delta;
            var isOverlapBottom = tooltipRect.bottom > bodyHeight - delta;

            if (isOverlapTop) {
                coords.top = delta;
            }

            if (isOverlapBottom) {
                coords.bottom = delta;
            }

            $tooltip.css(coords);
            if (options.isIE7 || options.isIE8) {
                return $tooltip.height(Math.min($tooltipContent[0] != null ? $tooltipContent[0].scrollHeight : void 0, 200));
            }
        }
    }

    function calcHomeBtnVisibility($viewport) {
        return setTimeout(function () {
            var $homeBtn = $viewport.find('.bt-home');

            var calcHeight;
            if ($homeBtn.hasClass('is-shown')) {
                var btn = $homeBtn[0];
                var style = btn.currentStyle || window.getComputedStyle(btn);
                calcHeight = btn.offsetHeight + parseInt(style.marginTop) + parseInt(style.marginBottom);
            } else {
                calcHeight = 0;
            }

            var showBtn = $viewport[0].offsetHeight < $viewport[0].scrollHeight - calcHeight;

            $homeBtn.toggleClass('is-shown', showBtn);
        }, 400);
    }

    function showTooltip(id, btnId) {
        if ($tooltip.data("id") !== id) {
            var content = document.getElementById("d" + id).innerHTML;
            $tooltip.data("id", id);
            $tooltipContent.html(content);
        }

        tooltipButton = document.getElementById(btnId);
        $tooltip.show();

        calcTooltipPosition();
    }

    function hideTooltip() {
        $tooltip.hide();
    }

    function toggleTooltip(id, btnId) {
        if ($tooltip.data("id") !== id || $tooltip.css("display") === "none") {
            showTooltip(id, btnId);
        } else {
            hideTooltip();
        }
    }

    function bindEvents() {
        var $asidePlaceholder = $('.js_aside_placeholder');
        var $menuItems = $('.js_menu_item');
        var $mobileSearch = $('.js_mobile_search_result');
        var $viewport = $('.js_main');
        var mobileMenuToggle = function (button) {
            if (!$mobileSearch.hasClass('is-hidden')) {
                $mobileSearch.addClass('is-hidden');
            }

            $asidePlaceholder.toggleClass('is-active');
            $(button).toggleClass('is-active');
        };

        $('.js_burger').on('click', function (event) {
            event.preventDefault();
            mobileMenuToggle(this);
        });

        $body.on('click', '.js_mobile_menu_deeper', function (event) {
            event.preventDefault();
            mobileMenuShowItem($(this).data('id'));
        });

        $body.on('click', '.bt-home', function (event) {
            event.preventDefault();

            $(".js_content, .js_main").animate({
                scrollTop: 0
            }, '500', 'swing');
        });

        $window.on('resize', function () {
            calcTooltipPosition();
            calcHomeBtnVisibility($('.js_main'));
            calcAsideWidth($asidePlaceholder);
        });

        $viewport.on("scroll", calcTooltipPosition);

        $body.on('click', 'a', function (event) {
            var $self = $(this);
            var url = $self.attr('href');

            if (url && url.indexOf('mailto') === 0) {
                return;
            }

            if ($self.hasClass('hyperlinktemplate') || $self.hasClass('namedhyperlinktemplate')) {
                if (url.indexOf('http') === 0 && url.indexOf('help.kaspersky.com') === -1) {
                    if (window.confirm(HELP.localization.ExternalLinkText)) {
                        return;
                    }

                    event.preventDefault();
                }

                return;
            }

            if (HELP.isAllInOne) {
                if (url) {
                    var hash = url.match( /(.*)\.html?$/ );
                    if (hash && hash[1]) {
                        scrollToHash(hash[1]);

                        $(".js_menu_item").removeClass('is-active');
                        var $menuItem = $self.closest('.js_menu_item');
                        setMenuItemActive($menuItem);

                        if (window.history && window.history.pushState) {
                            window.history.pushState({
                                url: url,
                                hash: hash[1]
                            }, document.title, location.pathname + '#' + hash[1]);
                        }

                        event.stopPropagation();
                        event.preventDefault();

                        trackPage();

                        return;
                    }
                }
            }

            if (window.matchMedia) {
                var mq = window.matchMedia('screen and (max-width: 767px)');
                if (mq.matches) {
                    var toggleSiblings = $self.siblings('.js_contents_toggle');
                    if (toggleSiblings.length > 0 && !$self.hasClass('js_menu_link_direct')) {
                        toggleSiblings.trigger('click');
                        event.preventDefault();
                        return;
                    }
                }
            }

            if (url && url !== '#') {
                var localUrlRegExp = /([\w_]+\.htm)[l]?(#[\w]+(.htm)?)?/i;
                var urlMatches = url.match(localUrlRegExp);
                var isLocalUrl = Boolean(urlMatches && (url === urlMatches[1] || (urlMatches[2] && urlMatches[3])));
                var isLocalAnchor = urlMatches && urlMatches[2] && !urlMatches[3];

                if ($self.closest('.js_search_result_wrapper')) {
                    setParam('searchWord', HELP.searchWord, true);
                }

                var $view = $('.js_tabs_viewport');

                /*if ($view.length) {
                    setParam('asideScrollPosition', $view[0].scrollTop);
                }*/

                var $opened_menus = $('.js_contents_toggle.is-toggled+.js_menu_link');

                if ($opened_menus.length) {
                    setParam('openedMenus', _.map($opened_menus, function (a) {
                        return $(a).attr('href');
                    }));
                }

                if (isLocalUrl) {
                    if ($tooltip) {
                        $tooltip.hide();
                    }

                    var targetUrl = url;

                    if (HELP.isMacProject) {
                        targetUrl = "./pgs/" + urlMatches[2].slice(1);
                        if (HELP.isFileProtocol) {
                            $self.attr('href', targetUrl);
                        }
                    }

                    if (!HELP.isFileProtocol) {
                        event.preventDefault();

                        setNavigation(HELP.isMacProject ? urlMatches[2].slice(1) : url);

                        loadContent(targetUrl, true, null, url);

                        if (window.matchMedia && mq && mq.matches) {
                            $('.js_aside_placeholder').removeClass('is-active');
                            $('.js_burger').removeClass('is-active');
                            $('.js_mobile_aside').addClass('is-hidden');
                        }

                        $('.js_home').toggleClass('is-active', HELP.nav.home.url === HELP.nav.currentPage.url);
                    }
                } else if (isLocalAnchor) {
                    if (window.matchMedia && mq && mq.matches) {
                        $('.js_aside_placeholder').removeClass('is-active');
                        $('.js_burger').removeClass('is-active');
                        $('.js_mobile_aside').addClass('is-hidden');
                    }
                }
            } else {
                event.preventDefault();
            }
        });

        $body.on('click', '.js_menu_link, .js_menu_link_direct', function () {
            var $self = $(this);
            delete HELP.searchWord;
            setParam('searchWord', null, true);

            if (!$self.prev().hasClass('js_contents_toggle')) {
                $menuItems.removeClass('is-active');
            }

            $self.children('.js_contents_toggle').addClass('is-toggled').siblings('.js_contents_level').slideDown();
            $self.closest('.js_menu_item').addClass('is-active');

            var href = $self.attr('href');
            var topicRegExp = /[\w_]+\.htm/;
            var hashRegExp = /#([\w_]+)/;

            if (href.indexOf('#') > -1) {
                var locationMatch = location.pathname.match(topicRegExp);
                var hrefMatch = href.match(topicRegExp);
                var hash = href.match(hashRegExp)[1];
                event.preventDefault();

                if (!(locationMatch.length && hrefMatch.length && locationMatch[0] === hrefMatch[0])) {
                    loadContent(href, true, function() {
                        if (hash) {
                            scrollToHash(hash);
                        }
                    });
                }

                if (hash) {
                    scrollToHash(hash);

                    if (!HELP.isMacProject && window.history && window.history.pushState) {
                        window.history.pushState({
                            url: href,
                            hash: hash
                        }, document.title, href);
                    }
                }
            }
        });

        $(window).on('popstate', function (event) {
            var state = event.originalEvent.state;

            if (state && state.url) {
                if (!HELP.isAllInOne) {
                    loadContent(state.url, false, scroll, state.originalUrl);
                    setNavigation(state.url);
                } else {
                    scroll();
                    trackPage();
                    updateBreadcrumb();
                }
            }

            checkPrintSectionLink();

            function scroll() {
                if (state.hash) {
                    $(".js_menu_item").removeClass('is-active');
                    var $menuItem = $(".js_menu_link[href='" + state.hash + ".htm']").closest('.js_menu_item');
                    autoScrollContents($menuItem);

                    scrollToHash(state.hash);
                }
            }
        });

        $body.on('click', '.js_modal_close', function () {
            $overlay.removeClass('is-visible');
        }).on('click', '.js_overlay', function (event) {
            if (!$(event.target).closest('.js_modal').length) {
                $overlay.removeClass('is-visible');
            }
        });

        if (!HELP.isAllInOne) {
            var xStart = null;
            var yStart = null;

            $body.on('touchstart', '.js_content', function(event) {

                var tableInTopic = $(event.target).closest('.tableintopic');
                if (tableInTopic.length && tableInTopic.width() > tableInTopic.parent().width()) {
                    return;
                }

                var touch = event.originalEvent.touches[0];
                if (touch) {
                    xStart = touch.clientX;
                    yStart = touch.clientY;
                }
            });

            $body.on('touchmove', '.js_content', function(event) {
                if (!xStart || !yStart) {
                    return;
                }

                var touch = event.originalEvent.touches[0];
                var xEnd = touch.clientX;
                var yEnd = touch.clientY;
                var xDiff = xStart - xEnd ;
                var yDiff = yStart - yEnd;

                if (Math.abs(xDiff) > Math.abs(yDiff) && Math.abs(xDiff) > 50) {
                    if (xDiff > 0 && HELP.nav.nextPage) {
                        $('.js_container').addClass('is-loading');
                        $('.js_main').addClass('swipe-left');
                        setTimeout(function () {
                            $('.js_next_link').click();
                        }, 350);
                    } else if(xDiff < 0 && HELP.nav.prevPage) {
                        $('.js_container').addClass('is-loading');
                        $('.js_main').addClass('swipe-right');
                        setTimeout(function () {
                            $('.js_prev_link').click();
                        }, 350);
                    }

                    xStart = null;
                    yStart = null;
                }
            });
        }
    }

    /*
    #* Langs switch --------------------------------------------------------------------------------------------------
     */
    function loadLangs() {
        HELP.currentLangId = document.documentElement.lang;

        if (!window.Langs) {
            $('.js_selector_mobile_langs').remove();
            return;
        }

        var currentId = 'no id';
        var currentName = 'no name';
        var locLangs = window.LangsLocalization;

        window.Langs.forEach(function(item) {
            if (locLangs && locLangs[item.id]) {
                item.name = locLangs[item.id];
            }

            if (item.id === document.documentElement.lang) {
                HELP.currentLangId = currentId = item.id;
                HELP.currentLangCode = currentName = item.name;
            } else if ((document.documentElement.lang === 'zh-CN' && item.id === 'zh-Hans') ||
                        (document.documentElement.lang === 'zh-TW' && item.id === 'zh-Hant')) {
                currentId = item.id;
                HELP.currentLangCode = currentName = item.name;
            }
        });

        if (~['ja-JP', 'ko-KR', 'zh-Hans', 'zh-Hant', 'zh-HantTW', 'zh-CN', 'zh-TW'].indexOf(HELP.currentLangId)) {
            HELP.searchWordMinLength = 2;
        }

        if (window.Langs.length < 2) {
            $('.js_selector_mobile_langs').remove();
            return;
        }

        var isMac = Boolean(window.location.pathname.match(/pgs\/[\w_\.]+/));
        var isMacTitle = Boolean(window.location.pathname.match(/title\.html?$/));
        var LangListHtml = LangListTemplateFunction({
            langs: window.Langs,
            currentLang: {
                name: currentName,
                id: currentId
            },
            localization: HELP.localization,
            isMac: isMac,
            isMacTitle: isMacTitle
        });

        $header.find('.js_header_lang_list').html(LangListHtml);
        $('.js_selector_mobile_langs').html(LangListHtml).find('.js_dropdown').addClass('dropdown_large');

        $body.on('click', '.js_lang_item', function (event) {
            event.preventDefault();

            var $element = $(this);
            var langId = $element.data('lang-id');
            var path = window.location.pathname;
            var currentFolder = window.location.pathname.match(/([\w]+|[\w\.-]+)(\/pgs)?\/[\w_-]+.html?/)[1];

            window.location.href  = path.replace("/" + currentFolder + "/", "/" + langId + "/");
        });
    }

    function triggerSearch() {
        if (HELP.helpers.searchTimeout) {
            clearTimeout(HELP.helpers.searchTimeout);
        }

        HELP.helpers.searchTimeout = setTimeout(function () {
            var searchWord = $('.js_search_text').val().trim();

            if (searchWord.length < HELP.searchWordMinLength) {
                delete HELP.searchWord;
                setParam('searchWord', null, true);

                searchWord = null;

                $('.js-menu-content, .js_search').show();
                $('.js-search-content, .js_search_clear').hide();
            } else {
                HELP.searchWord = searchWord;
                setParam('searchWord', searchWord, true);

                var resultObj = search(searchWord, HELP.searchWordMinLength);
                resultObj.localization = HELP.localization;
                resultObj.isMacProject = HELP.isMacProject;

                var searchResultsHtml = SearchResultsTemplateFunction(resultObj);

                $('.js-menu-content, .js_search').hide();
                $('.js-search-content, .js_search_clear').show();

                $('.js_search_result_wrapper').html(searchResultsHtml);
            }
        }, 300);
    }

    /*
    #* TOC on the left ------------------------------------------------------------------------------------------------------
     */
    function loadMenu(menu) {
        if (!window['Toc']) {
            return;
        }

        var flatMenu = [];
        var currentUrl = HELP.isAllInOne || HELP.isMacProject ?
            /#([\w_]+)/.exec(location.hash) : /\/([\w_]+\.\w+$)/.exec(location.pathname);
        var currentPage = {};
        var currentLevel, parentPage;

        if (currentUrl && currentUrl[1]) {
            currentUrl = currentUrl[1];
        }

        function buildMenu(menu, level, parentId) {
            var isCurrentMenuLevelActive;
            level = level || 0;

            _.each(menu, function (item) {
                item.level = level;
                if (item.url && item.url.match(/#|(title\.html?)/)) {
                    return;
                }

                if (item.hidden) {
                    return;
                }

                flatMenu.push(item);

                if (item.url === currentUrl || item.url === currentUrl + '.htm') {
                    item.isActive = true;
                    currentPage = item;
                    currentLevel = level;
                }

                if (item.children) {
                    item.childIsActive = buildMenu(item.children, level + 1, item.id);
                }

                if (item.isActive || item.childIsActive) {
                    isCurrentMenuLevelActive = true;
                }

                if (level > 0 && parentId) {
                    return item.parentId = parentId;
                }
            });

            return isCurrentMenuLevelActive;
        }

        buildMenu(menu);

        var i, j, ref, item, prevSection, nextSection;
        for (i = j = 0, ref = flatMenu.length - 1; 0 <= ref ? j <= ref : j >= ref; i = 0 <= ref ? ++j : --j) {
            item = flatMenu[i];
            if (item.url === currentUrl || item.url === currentUrl + '.htm') {
                if (i > 0) {
                    prevSection = flatMenu[i - 1];
                }

                if (i !== flatMenu.length - 1) {
                    nextSection = flatMenu[i + 1];
                }
            }
        }

        if (!currentPage.url) {
            nextSection = flatMenu[0];
        }

        window.HELP = $.extend(true, window.HELP, {
            nav: {
                menu: menu,
                flatMenu: flatMenu,
                home: flatMenu[0],
                currentPage: currentPage,
                currentLevel: currentLevel,
                parentPage: parentPage,
                prevPage: prevSection,
                nextPage: nextSection
            }
        });

        var $homeBtn = $body.find('.js_home');
        var $homeBtnLink = $homeBtn.find('.js_home_link');

        if (HELP.nav.home) {
            $homeBtnLink.attr('href', HELP.nav.home.url);
        }

        if (HELP.isAllInOne) {
            $(window).on('load', function() {
                setTimeout(function() {
                    scrollToHash(currentUrl);
                });
            });
        }

        buildLinks($container);

        var MenuHtml = MenuTemplateFunction({
            menu: menu,
            localization: HELP.localization,
            isMacProject: HELP.isMacProject
        });

        $aside.find('.js_menu').html(MenuHtml);

        $('.js_search_text').on('keyup', function (event) {
            if (event.keyCode === 27) {
                $(this).val('');
            } else {

            }
            triggerSearch();
        });

        $('.js_search_clear').on('click', function () {
            $('.js_search_text').val('');
            triggerSearch();

            $('.cont mark').contents().unwrap();
        });

        $('.js_mobile_search_button').on('click', function () {
            var $this = $(this);
            var $result = $('.js_mobile_search_result');
            var $input = $('.js_mobile_search_input');
            var $clearBtn = $('.js_mobile_search_clear');
            var isShow = !$input.hasClass('is-hidden');

            if (isShow) {
                if ($input.val().length > 0) {
                    var searchWord = $input.val().trim();
                    var resultObj = search(searchWord, HELP.searchWordMinLength);

                    resultObj.localization = HELP.localization;
                    resultObj.isMacProject = HELP.isMacProject;
                    var mobileSearchResultsHtml = MobileSearchResultsTemplateFunction(resultObj);

                    $result.html(mobileSearchResultsHtml).removeClass('is-hidden').addClass('is-active');

                    setTimeout(function () {
                        $('.js_aside_placeholder').removeClass('is-active');
                        $('.js_burger').removeClass('is-active');
                    });
                } else {
                    $result.removeClass('is-active').addClass('is-hidden');
                }

                /*$this.removeClass('is-active');
                $input.addClass('is-hidden');
                $clearBtn.addClass('is-hidden');*/
            } else {
                $input.removeClass('is-hidden');
                $clearBtn.removeClass('is-hidden');
                $this.addClass('is-active');

                if (!$result.is(':empty')) {
                    $result.removeClass('is-hidden').addClass('is-active');
                }

                setTimeout(function () {
                    $input.focus();
                }, 100);
            }
        });

        $('.js_mobile_search_clear').on('click', function () {
            var $searchBtn = $('.js_mobile_search_button');
            var $result = $('.js_mobile_search_result');
            var $input = $('.js_mobile_search_input');

            if ($input.val().length > 0) {
                $input.val('');
                HELP.searchWord = null;
                setParam('searchWord', null, true);

                $result.empty();
            }

            $searchBtn.removeClass('is-active');
            $result.addClass('is-hidden');
            $input.addClass('is-hidden');
            $(this).addClass('is-hidden');
        });

        var triggerMobileSearch = _.debounce(function() {
            $('.js_mobile_search_button').trigger('click');
        }, 350);

        $('.js_mobile_search_input').on('keyup', function (event) {
            if (event.keyCode === 13) {
                $('.js_mobile_search_button').trigger('click');
            // } else {
            //     triggerMobileSearch();
            }
        });

        $('.js_contents_toggle').on('click', function (event) {
            event.preventDefault();

            var $toggler = $(this);
            $toggler.toggleClass('is-toggled');

            if ($toggler.hasClass('is-toggled')) {
                $toggler.siblings('.contents').slideDown();
            } else {
                $toggler.siblings('.contents').slideUp();
            }
        });

        var openedClass = 'dropdown__list_opened';
        $('.js_dropdown_btn').on('click', function (event) {
            event.preventDefault();

            var $list = $(this).siblings('.js_dropdown_list');

            $('.js_dropdown_list').not($list).removeClass(openedClass);

            var maxHeight = html.clientHeight - $list.offset().top - this.clientHeight;

            if (HELP.isFirefox && $list.height() > maxHeight) {
                $list.addClass('dropdown__list_firefox-fix');
            }

            $list.css({
                maxHeight: maxHeight
            }).toggleClass(openedClass);

            $body.on('click.dropdown', function (event) {
                if (!$(event.target).closest('.js_dropdown').length) {
                    $list.removeClass(openedClass);
                    $body.off('click.dropdown');
                }
            });
        });

        bindEvents();
    }

    function loadContent(url, saveToHistory, callback, originalUrl) {
        if (saveToHistory == null) {
            saveToHistory = true;
        }

        var loadingTimeout = setTimeout(function () {
            $('.js_container').addClass('is-loading');
        }, 300);

        $.ajax(url, {
            crossDomain: true,
            dataType: "html",
            success: function (response) {
                if (saveToHistory && window.history && window.history.pushState) {
                    window.history.pushState({
                        url: url,
                        originalUrl: originalUrl
                    }, null, originalUrl || url);
                }

                var frameHtml = processHtml(response);
                var langMatches = frameHtml.match(/\slang="(.+?)"/);

                if ((langMatches != null ? langMatches.length : void 0) > 1 && langMatches[1] !== HELP.currentLangId) {
                    window.location = HELP.isMacProject ? originalUrl : url;
                    return;
                }

                var $cont = $(frameHtml).filter('.cont');
                $content.html($cont);

                $(".js_content, .js_main").animate({
                    scrollTop: 0
                }, '200', 'swing');

                title = $cont.find('.heading1, .heading2, .heading3, .heading4, .heading5').first().text();
                document.title = title;
                $(".js_menu_item").removeClass('is-active');

                var scrollToUrl = HELP.isMacProject ? originalUrl : url || HELP.nav.currentPage.url;
                var $menuItem = $(".js_menu_link[href='" + scrollToUrl + "']").closest('.js_menu_item');
                var $index = $('.js_index_literals');
                var $indexLinks = $index.find('.indexlink > a');
                $indexLinks.removeClass('is-active').filter("[href='" + url + "']").addClass('is-active');

                autoScrollContents($menuItem);
                processContent($container);

                checkPrintSectionLink();

                autotestHelperPostprocess();

				if (loadingTimeout) {
                    clearTimeout(loadingTimeout);
                }

                if (callback && typeof callback === 'function') {
                    callback();
                }

                $('.js_container').removeClass('is-loading');
                $('.js_main').removeClass('swipe-left swipe-right');

                trackPage();
                updateBreadcrumb();
            },
            error: function () {
                window.history.back();
                return window.location = url;
            }
        });
    }

    function processHtml(content, menu) {
        if (menu) {
            var AllInOneMenuHtml = AllInOneMenuTemplateFunction({
                menu: menu,
                localization: HELP.localization
            });
            var resultContent = $();

            menu.forEach(getContent);

            function getContent(item) {
                var id = item.url.match(/(\w+)\.html?/)[1];
                resultContent = resultContent.add('#' + id + ', [name=' + id + ']');

                if (item.children) {
                    item.children.forEach(getContent);
                }
            }

            content = AllInOneMenuHtml + $('<div/>').html(resultContent).html();
        }

        if (typeof content === 'string') {
            content = content.replace(/[™®]|&reg;|&#8482;|&#174;/g, '');

            if (HELP.isMacProject) {
                content = content.replace(/"title.htm#([\w]+.htm)"/gi,'"index.htm#$1"');
            }

            if (HELP.searchWord) {
                HELP.searchRegExp = new RegExp(escapeRegExp(HELP.searchWord) + '(?!([^<]+)?>)', "ig");
                content = content.replace(HELP.searchRegExp, "<mark>$&</mark>");
            }
        }

        return content;
    }

    function processContent($container) {
        $container.find('.links').remove();
        buildLinks($container);

        $container.find('.cont').toggleClass('mactitlepage', Boolean(location.pathname.match(/title\.htm/)));
        $container.find('style').remove();

        if (!HELP.isAllInOne) {
            $container.find('.popuponclick, .expandingblock').each(function () {
                var $self = $(this);
                var $toggler = $($self.attr('id').replace('d', '#h'));

                if ($toggler.length > 0) {
                    var $wrapper = $toggler.closest(".settingdescr");

                    if ($wrapper.length === 0) {
                        $wrapper = $toggler.parent();
                    }

                    $wrapper.append($self);

                    if ($self.hasClass('expandingblock')) {
                        $wrapper.addClass('expandingblock-wrapper');

                        var img = $toggler.next('img');

                        if (img.length) {
                            $toggler.append(img);
                        }
                    }
                }
            });
        }

        $container.find('.cont [title]').each(function(index, item) {
            var classes = item.className;
            var $item = $(item);

            if (!$item.is('img') &&
                classes.indexOf('hyperlinktemplate') === -1 &&
                classes.indexOf('crossreferencetemplate') === -1) {
                $item.removeAttr('title');
            }
        });

        $container.find('.hyperlinktemplate').each(function() {
            var $self = $(this);
            var $siblingLink = $self.next('span').find('.hyperlinktemplate');

            if ($siblingLink.length) {
                var text = $siblingLink.text();
                $self.append($siblingLink.parent().text(text));
            }

            if (HELP.isIE9 && $html.attr('dir') === 'rtl') {
                $self.html($self.html().replace(/([A-z0-9\s]{3,})/g, '$1&rlm;'));
            }
        });

        var $tables = $container.find(".tableintopic");
        $tables.wrap("<div class='tableintopic-wrapper'/>");
        var $tableRows = $tables.find('tr');
        $tableRows.filter(':nth-child(2n)').addClass('is-even');
        $tableRows.filter(':nth-child(2n+1)').addClass('is-odd');

        $container.find('.popuponhover, .popuponclick').each(function () {
            var $self = $(this);
            var $toggler = $($self.attr('id').replace('d', '#h'));

            if ($toggler.length > 0) {
                return $toggler.wrap("<div class='link-wrapper'/>").after($self);
            }
        });

        $container.find('.js_feedback_link').attr('href', getMailtoLink());

        /*$container.find('.program').each(function(index, item) {
            var $item = $(item);
            if (!$item.parent().hasClass('js_code')) {
                var siblings = $(item).nextUntil(':not(.program)').addBack();

                if (siblings.length > 1) {
                    siblings.wrapAll('<div class="js_code"></div>');
                    siblings.after('\n');
                }
            }
        });*/

        $container.find('.exampleheading, .examplebodytext, .exampleheadingblue2015, .examplebodytextblue2015')
            .parent().each(function(index, item) {
                $(item).addClass('example-wrapper');
            });

        // Begin KOS Styles

        $container.find('.programkos,.programcfgkos,.programmakefilekos,.programedlkos,.programidlkos,.programcdlkos,.programyamlkos,.programckos,.programjsonkos,.programxmlkos,.programbashkos,.programplaintextkos,.attentionbodykos,.programpythonkos,.programinicfgkos,.programsqlkos').each(function (index, item) {
            var $item = $(item);
            var itemClass = $item.attr('class');
            var checkWrapperClass = itemClass == 'attentionbodykos' ? 'attentionbodykos-wrapped' : 'js_code';
            var setWrapperClass;
            switch (itemClass) {
                case 'programkos':
                    setWrapperClass = 'js_code cpp bgprogram';
                    break;
                case 'programcfgkos':
                    setWrapperClass = 'js_code cpp bgprogram';
                    break;
                case 'programmakefilekos':
                    setWrapperClass = 'js_code cmake bgprogram';
                    break;
                case 'programedlkos':
                    setWrapperClass = 'js_code java bgprogram';
                    break;
                case 'programidlkos':
                    setWrapperClass = 'js_code java bgprogram';
                    break;
                case 'programcdlkos':
                    setWrapperClass = 'js_code java bgprogram';
                    break;
                case 'programyamlkos':
                    setWrapperClass = 'js_code yaml bgprogram';
                    break;
                case 'programckos':
                    setWrapperClass = 'js_code cpp bgprogram';
                    break;
                case 'programjsonkos':
                    setWrapperClass = 'js_code json bgprogram';
                    break;
                case 'programxmlkos':
                    setWrapperClass = 'js_code xml bgprogram';
                    break;
                case 'programbashkos':
                    setWrapperClass = 'js_code bash bgprogram';
                    break;
                case 'programplaintextkos':
                    setWrapperClass = 'js_code plaintext bgprogram';
                    break;
                case 'programpythonkos':
                    setWrapperClass = 'js_code python bgprogram';
                    break;
                case 'programinicfgkos':
                    setWrapperClass = 'js_code ini bgprogram';
                    break;
                case 'programsqlkos':
                    setWrapperClass = 'js_code sql bgprogram';
                    break;
                case 'attentionbodykos':
                    setWrapperClass = 'attentionbodykos-wrapped';
            }
            if (!$item.parent().hasClass(checkWrapperClass)) {
                var siblings = $(item).nextUntil(':not(.' + itemClass + ')').addBack();
                siblings.wrapAll('<div class="' + setWrapperClass + '"></div>');
                siblings.after('\n');
            }
        });

        // Definition List <dl>
        $container.find('ul.definitionlistterm').each(function (index, item) {
            var dl = document.createElement('dl');
            dl.innerHTML = item.innerHTML;
            item.parentNode.insertBefore(dl, item);
            item.parentNode.removeChild(item);
        });
        // Definition List Term <dt>
        $container.find('dl>li.definitionlistterm').each(function (index, item) {
            var dt = document.createElement('dt');
            dt.innerHTML = item.innerHTML;
            item.parentNode.insertBefore(dt, item);
            item.parentNode.removeChild(item);
        });
        // Definition List Description <dd>
        $($container.find('dl>dt>p.definitionlistdescription').get().reverse()).each(function (index, item) {
            var dd = document.createElement('dd');
            dd.innerHTML = item.innerHTML;
            item.parentNode.insertAdjacentElement('afterEnd', dd);
            item.parentNode.removeChild(item);
        });

        // End KOS Styles

        setTimeout(function() {
            hljs.configure({
                ignoreUnescapedHTML: true
            });
            $container.find('.js_code').each(function(index, item) {
                window.hljs.highlightElement(item);
                if (HELP.searchWord && HELP.searchRegExp) { // Recover <mark> tags after highlight, if any.
                    item.innerHTML = item.innerHTML.replace(HELP.searchRegExp, "<mark>$&</mark>");
                }
            });
        });

        $window.resize();
    }

    function buildLinks($container) {
        $pageTitle = $container.find('.heading1, .heading2, .heading3, .heading4').first();
        var $homeBtn = $container.find('.bt-home');
        var $prevBtnPlaceholder = $container.find('.js_prev_btn_placeholder');
        var $nextBtnPlaceholder = $container.find('.js_next_btn_placeholder');
        var PageTitleTemplateFunction = _.template(PageTitleTemplate);
        var BottomLinksTemlateFunctions = _.template(BottomLinksTemplate);
        var NextLinkTemplateTemlateFunctions = _.template(NextLinkTemplate);
        var PrevLinkTemplateTemlateFunctions = _.template(PrevLinkTemplate);

        if (HELP.nav) {
            var PageTitleHtml = PageTitleTemplateFunction({
                titleHtml: $pageTitle.html(),
                localization: HELP.localization
            });

            $pageTitle.html(PageTitleHtml);

            if (!HELP.isAllInOne) {
                var NextLinkHtml = NextLinkTemplateTemlateFunctions({
                    nextPage: HELP.nav.nextPage,
                    isMacProject: HELP.isMacProject
                });

                var PrevLinkHtml = PrevLinkTemplateTemlateFunctions({
                    prevPage: HELP.nav.prevPage,
                    isMacProject: HELP.isMacProject
                });

                $prevBtnPlaceholder.html(PrevLinkHtml);
                $nextBtnPlaceholder.html(NextLinkHtml);

                if ($homeBtn.length === 1 && (!HELP.isMacProject || window.location.hash)) {
                    $homeBtn = $homeBtn.first();

                    if (window.reviewDate) {
                        var reviewDate = new Date(window.reviewDate);

                        if (HELP.lessThanIE11) {
                            reviewDate = formatDate(reviewDate);
                        } else {
                            var locale = HELP.currentLangId === 'en-EN' ? 'en' : HELP.currentLangId;
                            reviewDate = reviewDate.toLocaleString(locale, {
                                year: 'numeric',
                                month: 'short',
                                day: 'numeric'
                            });
                        }
                    }

                    var separator = {
                        'ja-JP': '、',
                        'fa-IR': '،',
                        'ar-AE': '،'
                    };

                    var BottomLinksHtmls = BottomLinksTemlateFunctions({
                        homeBtnHtml: $homeBtn[0].outerHTML,
                        localization: HELP.localization,
                        serviceInfo: {
                            id:  HELP.isMacProject ? window.location.hash.match(/#(\w+)\.htm/)[1] : window.location.pathname.match(/\/(\w+)\.htm/)[1],
                            reviewDate: reviewDate,
                            separator: separator[HELP.currentLangId] || ','
                        }
                    });

                    $homeBtn.replaceWith(BottomLinksHtmls);
                } else {
                    $homeBtn.remove();
                }
            } else {
                $('.js_all_in_one_header').text(HELP.localization.TableOfContents);
            }
        }
    }

    function autoScrollContents($target, animateSpeed) {
        if (animateSpeed == null) {
            animateSpeed = 2;
        }

        if (!$target || !$target.length) {
            return;
        }

        var $view = $('.js_tabs_viewport');
        var $menuItem = $view.find('.js-menu-content:not(.is-hidden)').find($target);

        if (!$menuItem.length || !$view.length) {
            return;
        }

        setMenuItemActive($menuItem);

        var targetTop = getParam('asideScrollPosition');
        if (targetTop) {
            setParam('asideScrollPosition', null);
            var isFromLocalStorage = true;
        } else {
            var view = $view[0];
            var delta1 = view.clientHeight / 5;
            var delta2 = view.clientHeight / 4;
            var isOverBottom = $menuItem.offset().top > view.clientHeight + $view.offset().top;
            var isOverTop = $menuItem.offset().top < $view.offset().top + $menuItem[0].offsetHeight;
            var targetDelta = $menuItem.offset().top - $view.offset().top;
            targetTop = view.scrollTop + targetDelta;
        }

        var scrollingDuration = Math.min(animateSpeed * Math.abs(targetDelta), 1500);
        if (isFromLocalStorage || isOverTop || isOverBottom) {
            $view.animate({
                scrollTop: targetTop
            }, scrollingDuration, 'swing');
        }
    }

    function loadVersions() {
        if (!HELP.localization.products_b2c || !HELP.localization.products_b2b) {
            return;
        }

        var url = window.location.pathname.split('/');
		if (url[1].toLowerCase() == 'help') {
			url.splice(1, 1); // browser url may have prepending /help/
		}
        var productKey = url[1] && url[1];
        var currentVersionUrl = url[2];
        var allProducts = HELP.localization.products_b2c.concat(HELP.localization.products_b2b).concat(HELP.localization.products_kos || []);
		allProducts.forEach(function(product) {
			product.url = product.url.replace(/\/?help\//gi, ''); // product urls in json may have prepending help/
		});

        function productHasSelectedVersion(item) {
            return _.some(item.versions, function (versionItem) {
                let versionNum = versionItem.url.split('/')[0].toLowerCase();
                return versionNum === currentVersionUrl.toLowerCase()
            });
        }

        var product = _.filter(allProducts, function (item) {
            return item.url.toLowerCase() === productKey.toLowerCase()

                // workaround for #4073658
                && productHasSelectedVersion(item);
        })[0];

        if (!product) { // if workaround above fails, try original code
            var product = _.filter(allProducts, function (item) {
                return item.url.toLowerCase() === productKey.toLowerCase();
            })[0];
        }

        var $productLogo = $('.js_product_logo');

        if (!product) {
            return;
        }

        $productLogo.addClass('top-bar__product_' + product.icon);

        var currentVersion;
        _.some(product.versions, function(item) {
            if (item.url.split('/')[0] === currentVersionUrl) {
                currentVersion = item;
                return true;
            }
            return false;
        });

        if (currentVersion && product.versions.length > 1) {
            var VersionsResultsHtml = VersionsTemplateFunction({
                currentVersion: currentVersion,
                versions: product.versions
            });

            $header.find('.js_header_versions_list').html(VersionsResultsHtml);
            $('.js_selector_mobile_versions')
                .html(VersionsResultsHtml).find('.js_dropdown').addClass('dropdown_large');

            $header.on('click', '.js_version_item', function (event) {
                event.preventDefault();

                var versionUrl = $(this).data('version-url');
                /*var path = window.location.pathname.split('/');
                var currentPage = path[path.length - 1];

                window.location.href  = '/' + productKey + '/' + versionUrl + '/' + currentPage;*/
                var main_url = $('a.header__title-link').attr('href');
                if( ! main_url.endsWith('/')) {
                    main_url += '/'
                }
                window.location.href =  main_url + productKey + '/' + versionUrl;
            });
        } else {
            $('.js_selector_mobile_versions').remove();
        }
    }

    function calcAsideWidth(aside) {
        aside.children().css({
            width: aside.width(),
            top: $('.js_header_placeholder').height()
        });
    }

    function getMailtoLink() {
        var link = 'mailto:?subject=Shared from Kaspersky Online Help: "' +
            window.parsedMainTitle + ' - ' + window.document.title + '"&body=' + window.parsedMainTitle +
            ' - ' + window.document.title + '\n' + window.location.href;
        return encodeURI(link);
    }

    function findSection(menuList, sectionUrl) {
        var result =[];

        menuList.forEach(function(item) {
            if (item.url === sectionUrl) {
                result.push(item);
            } else if (item.children) {
                result = result.concat(findSection(item.children, sectionUrl));
            }
        });

        return result;
    }

    function scrollToHash(hash) {
        var $target = $("[name='" + hash + "'], #" + hash);
        var $main = $('.js_main');

        if ($target.length) {
            $main.animate({
                scrollTop: $target[0].offsetTop - $main[0].offsetTop
            }, '500', 'swing');

            document.documentElement.scrollTop = 0;
        }
    }

    function escapeRegExp(text) {
        return text.toString().replace(/[-[\]{}()*+?.,\\^$|#\s]/g, "\\$&");
    }

    function trackPage() {
        if (window._gaq) {
            window._gaq.push(['_trackPageview', location.pathname + location.search + location.hash]);
        }
    }

    function updateBreadcrumb() {
        var breadPage = HELP.nav.currentPage;
        if (!breadPage.level || breadPage.level == 0) {
            return;
        }
        var breadHtml = breadPage.text;
        while (breadPage.parentId) {
            breadPage = HELP.nav.flatMenu.filter(function (b) { return b.id == breadPage.parentId })[0];
            urlPrefix = HELP.isMacProject ? "index.htm#" : "";
            breadHtml = "<a class=\"breadcrumbLink\" href=\"" + urlPrefix + breadPage.url + "\">" + breadPage.text + "</a>&ensp;>&ensp;" + breadHtml;
        }
        document.getElementsByClassName("cont")[0].insertAdjacentHTML('afterbegin', "<div class=\"breadcrumbBlock\">" + breadHtml + "</div>");
    }

    function setNavigation(url) {
        _.each(HELP.nav.flatMenu, function (item, index) {
            if (item.url === url) {
                HELP.nav.currentPage = item;

                if (index > 0) {
                    HELP.nav.prevPage = HELP.nav.flatMenu[index - 1];
                } else {
                    HELP.nav.prevPage = null;
                }

                if (index !== HELP.nav.flatMenu.length - 1) {
                    HELP.nav.nextPage = HELP.nav.flatMenu[index + 1];
                } else {
                    HELP.nav.nextPage = null;
                }
            }
        });
    }

    function handleCookiePolicy(cookiePolicy) {
        if (window.loadGA) {
            window.loadGA();
        }

        //if (getParam('hideCookiePolicy')) {
        if (true) {
            cookiePolicy.remove();
            return;
        }

        cookiePolicy.find('.js_cookie_policy_btn').on('click', function() {
            cookiePolicy.remove();
            setParam('hideCookiePolicy', true);
        });
    }

    function setMenuItemActive($menuItem) {
        $menuItem
            .addClass('is-active')
            .parents('.js_contents_level')
            .add($menuItem.children('.js_contents_level'))
            .slideDown()
            .siblings('.js_contents_toggle')
            .addClass('is-toggled');
    }

    function formatDate(date) {
        var dd = date.getDate();
        var mm = date.getMonth() + 1;
        var yyyy = date.getFullYear();

        if (dd < 10){
            dd = '0' + dd;
        }

        if (mm < 10){
            mm = '0' + mm;
        }

        return dd + '.' + mm + '.' + yyyy;
    }

    function checkPrintSectionLink() {

        if (HELP.isAllInOne) {
            return;
        }

        var link = $('.js_print_section');

        if (HELP.isMacProject && !window.location.hash) {
            link.hide();
        } else {
            link.show();

            var currentUrl = HELP.isMacProject ?
                window.location.hash.match(/#((\w+)\.htm)/)[1] : window.location.pathname.match(/\/(\w+\.\w+)$/)[1];

            link.attr('href', (HELP.isMacProject ? 'pgs/' : '') + 'all-in-one.htm?sectionUrl=' + currentUrl);
        }
    }

	function autotestHelperPostprocess() {
		if (document.title=="AVAILABILITY OF KASPERSKY SECURITY CLOUD") {
			document.getElementsByClassName("js_content")[0].dataset.atSelector = "at-countryRestrictions"; //requested by KSCloud autotesters
		} else if (document.getElementsByClassName("js_content")[0].dataset.atSelector == "at-countryRestrictions") {
			document.getElementsByClassName("js_content")[0].removeAttribute("data-at-selector");
		}
	}

    /*
    #* Rendering starts here --------------------------------------------------------------------------------------------------
     */
    function firstRender() {
        var $footer, $menuItem;
        var sectionUrl = location.search.split('sectionUrl=')[1];
        var menu = window['Toc'];

        if (sectionUrl) {
            menu = findSection(menu, sectionUrl);
        }

        HELP.localization = window.Localization || {};
        HELP.customization = window.Customization || {};
        HELP.pdfOptions = window.PdfOptions || {};

        $html.toggleClass('ie7', HELP.isIE7).toggleClass('ie8', HELP.isIE8)
             .toggleClass('ie9', HELP.isIE9).toggleClass('ie10', HELP.isIE10)
             .toggleClass('is-opera-mini', HELP.isOperaMini)
             .toggleClass('is-offline', HELP.isFileProtocol);

        if (window['HelpViewer'] !== void 0 && window.sessionStorage && JSON.parse && window.sessionStorage.getItem('isTOCHidden') && JSON.parse(window.sessionStorage.getItem('isTOCHidden'))) {
            $html.removeClass('is-menu-shown');
        }

        if (location.hostname === 'localhost' || location.hostname === 'stage.help.kaspersky.com') {
            $html.addClass('is-test-mode');
        }

        var containerHtml = ContainerTemplateFunction({
            localization: HELP.localization
        });

        var parsedMainTitle = window.parsedMainTitle;
        var titlePreffix;

        if (window.Customization) {
            titlePreffix = HELP.customization.CompanyName;
            parsedMainTitle = HELP.customization.ProductName;
        } else if (parsedMainTitle.indexOf('Kaspersky ') === 0) {
            titlePreffix = parsedMainTitle.slice(0, 9);
            parsedMainTitle = parsedMainTitle.slice(10, parsedMainTitle.length);
        } else if (parsedMainTitle.indexOf('KasperskyOS') === 0) {
            titlePreffix = parsedMainTitle.slice(0, 11);
            parsedMainTitle = parsedMainTitle.slice(12, parsedMainTitle.length);
        }

        var headerHtml = HeaderTemplateFunction({
            titlePreffix: titlePreffix || '',
            title: parsedMainTitle,
            localization: HELP.localization,
            customization: HELP.customization,
            isAllInOne: HELP.isAllInOne,
            isOffline: HELP.isFileProtocol,
            isMacProject: HELP.isMacProject,
            pdfOptions: HELP.pdfOptions
        });

        var footerHtml = FooterTemplateFunction({
            localization: HELP.localization,
            customization: HELP.customization
        });

        var asideHtml = AsideTemplateFunction({
            localization: HELP.localization
        });

        if (window.Customization) {
            var customStyles = CustomStylesTemplateFunction({
                customization: HELP.customization
            });

            $head.append(customStyles);
        }

        $container = $(containerHtml);
        $header = $(headerHtml);
        $footer = $(footerHtml);
        $aside = $(asideHtml);

        var $headerPlaceholder = $container.find('.js_header_placeholder');
        var $asidePlaceholder = $container.find('.js_aside_placeholder');
        var $footerPlaceholder = $container.find('.js_footer_placeholder');

        $headerPlaceholder.append($header);
        $asidePlaceholder.append($aside);
        $footerPlaceholder.append($footer);
        $content = $container.find('.js_content');
        $content.append(processHtml($body.html(), sectionUrl ? menu : null));
        $body.html($container);
        processContent($container);
        $body.addClass('js_tabs');

        var $viewport = $('<meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">');
        $head.append($viewport);
        $html.find('base').remove();

        if ($.inArray($html.attr('lang'), ['ar-AE', 'fa-IR']) > -1) {
            $html.attr('dir', 'rtl');
        } else {
            $html.attr('dir', 'ltr');
        }

        FastClick.attach(document.body);

        $overlay = $body.find('.js_overlay');
        loadLangs();
        loadVersions();
        loadMenu(menu);
        handleCookiePolicy($body.find('.js_cookie_policy'));
        checkPrintSectionLink();

        if (HELP.searchWord) {
            $body.find('.js_search_text').val(HELP.searchWord);
            triggerSearch();
        }

        if (HELP.nav.currentPage && HELP.nav.currentPage.url) {
            $menuItem = $(".js_menu_link[href='" + HELP.nav.currentPage.url + "']").closest('.js_menu_item');
            autoScrollContents($menuItem, 0);
        }

        var localUrlRegExp = /([\w_]+\.htm)[l]?(#[\w]+(.htm))?/i;
        var urlMatches = window.location.href.match(localUrlRegExp);
        if (HELP.isMacProject) {
            if (urlMatches[2] && urlMatches[3]) {
                loadContent("./pgs/" + urlMatches[2].slice(1), false, null, urlMatches[0]);
            } else {
                loadContent("./pgs/" + HELP.nav.nextPage.url, false, null, urlMatches[0] + '#' + HELP.nav.nextPage.url);
            }
        } else if (window.history && window.history.replaceState && !HELP.isAllInOne) {
            var initialUrl = /\/([\w_-]+\.\w+$)/.exec(location.pathname)[1]+location.hash;
            window.history.replaceState({
                url: initialUrl
            }, null, initialUrl);
        }

        $modalContent = $overlay.find('.js_modal_content');
        $modalHeader = $overlay.find('.js_modal_header');
        $tooltip = $body.find('.js_tooltip');
        $tooltipContent = $body.find('.js_tooltip_content');

        if (HELP.isIE7 || HELP.isIE8) {
            setTimeout(function () {
                calcAsideWidth($asidePlaceholder);
            }, 500);
        }

        $(".downloadhyperlinktemplate").attr("download", "").addClass("hyperlinktemplate");

        autotestHelperPostprocess();
        updateBreadcrumb();

        var kasperskyFont100 = new window.FontFaceObserver('KasperskySans', { weight: 100 });
        var kasperskyFont300 = new window.FontFaceObserver('KasperskySans', { weight: 300 });
        var kasperskyFont600 = new window.FontFaceObserver('KasperskySans', { weight: 600 });
        var kasperskyFont700 = new window.FontFaceObserver('KasperskySans', { weight: 700 });

        Promise.all([
            kasperskyFont100.load(null, 3000),
            kasperskyFont300.load(null, 3000),
            kasperskyFont600.load(null, 3000),
            kasperskyFont700.load(null, 3000)
        ]).then(showPage, showPage);

        function showPage() {
            $html.css('visibility', 'visible');
            $window.resize();

            if (HELP.isAllInOne) {
                window.print();
            }
        }
    }

    firstRender();
});
