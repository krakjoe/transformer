<?php
class Foo {

	<<contract($arg > 2, $arg != 9)>>
	public function bar($arg) {
		return $arg;
	}
}

$foo = new Foo();
$foo->bar(9);
?>
