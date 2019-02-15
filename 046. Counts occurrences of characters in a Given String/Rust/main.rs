fn chars_count(string: &String, value: char) -> usize {
	let mut result: usize = 0;
	for ch in string.chars() {
		if ch == value {
			result += 1;
		}
	}
	return result;
}

fn length(string: &String) -> usize {
	return string.chars().count() - chars_count(string, '\n') - chars_count(string, '\r');
}

fn main() {
	println!("Enter string: ");
	let mut input = String::new();
	std::io::stdin().read_line(&mut input).expect("Failed to read line");

	let mut character = String::new();

	println!("{}", length(&input));

	while length(&character) != 1 {
		character.clear();
		println!("Enter single character: ");
		std::io::stdin().read_line(&mut character).expect("Failed to read line");
	}

	let ch: char = character.chars().next().unwrap();
	println!("Number of occurences: {}", chars_count(&input, ch));
}


