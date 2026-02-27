# Droy Runtime - Live UI Preview

Droy Runtime is a live preview environment for Droy Language UI components. It allows you to design and preview user interfaces in real-time.

## Features

### Visual Component Library
- **30+ UI Components** - Buttons, inputs, cards, alerts, and more
- **Drag & Drop** - Drag components from library to code editor
- **Live Preview** - See changes instantly as you type
- **Responsive Testing** - Test on desktop, tablet, and mobile views

### Component Categories

#### Basic Components
- Button - Interactive buttons with variants
- Input - Text input fields
- Text - Typography elements
- Image - Image display
- Icon - Font Awesome icons

#### Form Components
- Form - Form containers
- Checkbox - Checkable options
- Radio - Single selection options
- Select - Dropdown selections
- Textarea - Multi-line text input

#### Layout Components
- Container - Content containers
- Row - Horizontal layouts
- Column - Vertical layouts
- Card - Content cards
- Divider - Visual separators

#### Navigation Components
- Navbar - Navigation bars
- Tabs - Tabbed interfaces
- Breadcrumb - Navigation breadcrumbs
- Pagination - Page navigation

#### Feedback Components
- Alert - Status messages
- Modal - Dialog boxes
- Progress - Progress indicators
- Toast - Notification toasts
- Badge - Status badges

#### Data Display
- Table - Data tables
- List - Item lists
- Timeline - Event timelines
- Chart - Data visualizations

## Usage

### Opening the Runtime
```bash
# Open runtime directly
open runtime/index.html

# Or serve with a local server
cd runtime
python -m http.server 8080
# Then open http://localhost:8080
```

### Creating a UI

#### Method 1: Using Templates
1. Select a component template from dropdown
2. Customize properties in the Properties panel
3. See live preview update automatically

#### Method 2: Drag and Drop
1. Drag component from library
2. Drop onto code editor
3. Component code is inserted automatically

#### Method 3: Manual Coding
1. Type Droy code directly in editor
2. Preview updates in real-time

### Example: Creating a Button

```droy
block: key('myButton') {
    sty {
        set type = 'button'
        set text = 'Click Me'
        set variant = 'primary'
        set onClick = 'handleClick()'
    }
}
```

### Example: Creating a Form

```droy
block: key('contactForm') {
    sty {
        set type = 'form'
        set title = 'Contact Us'
    }
    
    block: key('nameField') {
        sty {
            set type = 'input'
            set label = 'Name'
            set placeholder = 'Your name'
        }
    }
    
    block: key('emailField') {
        sty {
            set type = 'input'
            set label = 'Email'
            set type = 'email'
        }
    }
    
    block: key('submitBtn') {
        sty {
            set type = 'button'
            set text = 'Submit'
            set variant = 'success'
        }
    }
}
```

## Component Properties

Each component has configurable properties:

### Button Properties
- `text` - Button label
- `variant` - Color variant (primary, secondary, success, danger)
- `size` - Button size (small, medium, large)
- `disabled` - Disabled state
- `onClick` - Click handler

### Input Properties
- `label` - Field label
- `placeholder` - Placeholder text
- `type` - Input type (text, email, password, number)
- `required` - Required field
- `onChange` - Change handler

### Card Properties
- `title` - Card title
- `content` - Card content
- `image` - Image URL
- `footer` - Footer text

### Alert Properties
- `message` - Alert message
- `variant` - Alert type (info, success, warning, error)
- `dismissible` - Can be closed

## View Modes

Test your UI on different devices:
- **Desktop** (900px width)
- **Tablet** (768px width)
- **Mobile** (375px width)

## Panels

### Properties Panel
Edit component properties in real-time:
- Basic properties
- Event handlers
- Custom styles

### Events Panel
Configure component events:
- Click handlers
- Change handlers
- Focus handlers

### Styles Panel
Add custom CSS:
```css
.my-element {
    background: #f0f0f0;
    padding: 20px;
}
```

### Generated Code Panel
View the generated Droy code for your UI.

## Integration with Droy Studio

The Runtime integrates with Droy Studio IDE:

1. Edit code in Droy Studio
2. Switch to Runtime tab
3. See live preview
4. Copy generated code back to project

## Browser Compatibility

- Chrome 80+
- Firefox 75+
- Safari 13+
- Edge 80+

## Project Structure

```
runtime/
├── index.html          # Main runtime interface
├── runtime.css         # Runtime styles
├── runtime.js          # Runtime logic
└── README.md           # This file
```

## License

MIT License - See LICENSE file for details
