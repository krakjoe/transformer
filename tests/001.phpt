--TEST--
Check for transformer operation
--SKIPIF--
<?php if (!extension_loaded("transformer")) print "skip"; ?>
--FILE--
<?php
class Foo {

	<<contract($arg != 9)>>
	public function bar($arg) {
		return $arg;
	}
}

$foo = new Foo();
$foo->bar(9);
?>
--EXPECTF--
Warning: assert(): assert($arg != 9) failed in %s on line 5

