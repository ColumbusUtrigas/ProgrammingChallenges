use std::io::Write;

fn remove_spaces(string: &String) -> String {
	let mut result: String = String::with_capacity(string.len());
	for ch in string.chars() {
		if ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' {
			result.push(ch);
		}
	}
	return result;
}

fn is_number(string: &String) -> bool {
	for ch in string.chars() {
		match ch {
			'0'...'9' => {},
			'.' => {},
			_ => return false
		}
	}

	return true;
}

fn to_numbers(string: String) -> Vec<f64> {
	let mut result: Vec<f64> = Vec::new();
	let mut number: String = String::new();

	for ch in string.chars() {
		if ch == ',' {
			if is_number(&number) {
				result.push(number.parse::<f64>().expect("Couldn't parse string to float"));
				number.clear();
			}
			continue;
		}

		match ch {
			'0'...'9' => number.push(ch),
			'.' => number.push(ch),
			_ => {}
		}
	}

	if number.len() > 0 && is_number(&number) {
		result.push(number.parse::<f64>().expect("Couldn't parse string to float"));
	}

	return result;
}

fn main() {
	let mut first_vector: String = String::new();
	let mut second_vector: String = String::new();

	print!("Enter first vector (x, y, z): ");
	std::io::stdout().flush().expect("Couldn't flush output");
	std::io::stdin().read_line(&mut first_vector).expect("Couldn't read first vector");

	print!("Enter second vector (x, y, z): ");
	std::io::stdout().flush().expect("Couldn't flush output");
	std::io::stdin().read_line(&mut second_vector).expect("Couldn't read second vector");

	first_vector = remove_spaces(&first_vector);
	second_vector = remove_spaces(&second_vector);

	let vec1 = to_numbers(first_vector);
	let vec2 = to_numbers(second_vector);

	if vec1.len() != 3 || vec2.len() != 3 {
		println!("Error: please, enter two valid 3-dimensional vectors");
		return;
	}

	println!("");
	println!("Dot product: {}", vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2]);
	println!("Cross product: {}, {}, {}", vec1[1] * vec2[2] - vec1[2] * vec2[1],
	                                      vec1[2] * vec2[0] - vec1[0] * vec2[2],
	                                      vec1[0] * vec2[1] - vec1[1] * vec2[0]);
}


